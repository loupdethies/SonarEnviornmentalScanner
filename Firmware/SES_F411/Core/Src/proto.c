/**
 ******************************************************************************
 * @file    proto.c
 * @brief   Binaeres Host-Protokoll ueber USB-CDC
 ******************************************************************************
 */
#include <string.h>
#include "proto.h"
#include "sonar.h"
#include "stusb4500.h"
#include "usbd_cdc_if.h"

/* ------------------------------- RX-Ringpuffer --------------------------- */
#define RX_BUF_SIZE 256u
static volatile uint8_t  s_rx_buf[RX_BUF_SIZE];
static volatile uint32_t s_rx_head;
static volatile uint32_t s_rx_tail;

void proto_init(void)
{
    s_rx_head = 0u;
    s_rx_tail = 0u;
}

void proto_rx(const uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0u; i < len; i++) {
        uint32_t next = (s_rx_head + 1u) % RX_BUF_SIZE;
        if (next == s_rx_tail) {
            break;                      /* Ueberlauf: Rest verwerfen        */
        }
        s_rx_buf[s_rx_head] = data[i];
        s_rx_head = next;
    }
}

static uint32_t rx_avail(void)
{
    return (s_rx_head + RX_BUF_SIZE - s_rx_tail) % RX_BUF_SIZE;
}

static uint8_t rx_peek(uint32_t off)
{
    return s_rx_buf[(s_rx_tail + off) % RX_BUF_SIZE];
}

static void rx_drop(uint32_t n)
{
    s_rx_tail = (s_rx_tail + n) % RX_BUF_SIZE;
}

/* ------------------------------- TX-Hilfen ------------------------------- */
static void cdc_send_blocking(const uint8_t *buf, uint32_t len)
{
    uint32_t off = 0u;
    while (off < len) {
        uint32_t chunk = len - off;
        if (chunk > 4096u) {
            chunk = 4096u;
        }
        uint32_t t0 = HAL_GetTick();
        while (CDC_Transmit_FS((uint8_t *)&buf[off], (uint16_t)chunk) == USBD_BUSY) {
            if ((HAL_GetTick() - t0) > 500u) {
                return;                 /* Host liest nicht - abbrechen     */
            }
        }
        off += chunk;
    }
}

void proto_send2(uint8_t type, const uint8_t *hdr, uint32_t hdr_len,
                 const uint8_t *payload, uint32_t len)
{
    uint8_t  frame[7];
    uint32_t total = hdr_len + len;

    frame[0] = SES_FRAME_MAGIC0;
    frame[1] = SES_FRAME_MAGIC1;
    frame[2] = type;
    frame[3] = (uint8_t)(total);
    frame[4] = (uint8_t)(total >> 8);
    frame[5] = (uint8_t)(total >> 16);
    frame[6] = (uint8_t)(total >> 24);

    cdc_send_blocking(frame, sizeof(frame));
    if ((hdr != NULL) && (hdr_len > 0u)) {
        cdc_send_blocking(hdr, hdr_len);
    }
    if ((payload != NULL) && (len > 0u)) {
        cdc_send_blocking(payload, len);
    }
}

void proto_send(uint8_t type, const uint8_t *payload, uint32_t len)
{
    proto_send2(type, NULL, 0u, payload, len);
}

static void send_error(uint8_t code)
{
    proto_send(SES_RSP_ERROR, &code, 1u);
}

/* ------------------------------- Befehle --------------------------------- */
static void cmd_version(void)
{
    uint8_t v[4] = { SES_FW_VERSION_MAJOR, SES_FW_VERSION_MINOR,
                     SES_FW_VERSION_PATCH, 0u };
    proto_send(SES_RSP_VERSION, v, sizeof(v));
}

static void cmd_pdstatus(void)
{
    stusb4500_status_t st;
    uint8_t p[10];

    if (stusb4500_read_status(&st) != HAL_OK) {
        send_error(0x02u);
        return;
    }
    p[0] = st.attached ? 1u : 0u;
    p[1] = st.device_id;
    memcpy(&p[2], &st.rdo_raw, 4u);
    memcpy(&p[6], &st.current_ma, 2u);
    memcpy(&p[8], &st.max_current_ma, 2u);
    proto_send(SES_RSP_PDSTATUS, p, sizeof(p));
}

static void send_measurement(void)
{
    /* Header: u32 fs, u16 nPulses, u32 nPairs -> danach interleavte Daten  */
    uint8_t  hdr[10];
    uint32_t fs = SES_ADC_FS_HZ;

    memcpy(&hdr[0], &fs, 4u);
    memcpy(&hdr[4], &g_sonar.n_pulses, 2u);
    memcpy(&hdr[6], &g_sonar.n_pairs, 4u);

    proto_send2(SES_RSP_MEASUREMENT, hdr, sizeof(hdr),
                (const uint8_t *)g_adc_buf,
                g_sonar.n_pairs * SES_ADC_CHANNELS * 2u);
}

static void proto_exec(void)
{
    uint8_t cmd = rx_peek(0u);

    switch (cmd) {
    case SES_CMD_VERSION:
        rx_drop(1u);
        cmd_version();
        break;

    case SES_CMD_MEASURE: {
        if (rx_avail() < 7u) {
            return;                     /* Parameter noch unvollstaendig    */
        }
        uint16_t n_pulses = (uint16_t)rx_peek(1u) | ((uint16_t)rx_peek(2u) << 8);
        uint32_t n_pairs  = (uint32_t)rx_peek(3u) | ((uint32_t)rx_peek(4u) << 8) |
                            ((uint32_t)rx_peek(5u) << 16) | ((uint32_t)rx_peek(6u) << 24);
        rx_drop(7u);
        if (n_pairs == 0u) {
            n_pairs = SES_DEFAULT_SAMPLE_PAIRS;
        }
        if (n_pulses == 0u) {
            n_pulses = SES_TX_DEFAULT_PULSES;
        }
        if (sonar_measure_start(n_pulses, n_pairs) != HAL_OK) {
            send_error(0x01u);
        }
        break;
    }

    case SES_CMD_STREAM_START:
        rx_drop(1u);
        if (sonar_stream_start() != HAL_OK) {
            send_error(0x01u);
        }
        break;

    case SES_CMD_STOP:
        rx_drop(1u);
        sonar_stop();
        break;

    case SES_CMD_BOOST: {
        if (rx_avail() < 2u) {
            return;
        }
        uint8_t on = rx_peek(1u);
        rx_drop(2u);
        HAL_GPIO_WritePin(SES_BOOST_EN_PORT, SES_BOOST_EN_PIN,
                          (on != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    }

    case SES_CMD_PDSTATUS:
        rx_drop(1u);
        cmd_pdstatus();
        break;

    default:
        rx_drop(1u);                    /* Unbekanntes Byte verwerfen       */
        break;
    }
}

void proto_task(void)
{
    /* 1. Eingehende Befehle */
    if (rx_avail() > 0u) {
        proto_exec();
    }

    /* 2. Fertige Einzelmessung ausliefern */
    if (sonar_poll_done()) {
        g_sonar.done = false;
        send_measurement();
    }

    /* 3. Streaming-Haelften ausliefern */
    if ((g_sonar.stream_ready & 0x01u) != 0u) {
        g_sonar.stream_ready &= (uint8_t)~0x01u;
        proto_send(SES_RSP_STREAM_CHUNK, (const uint8_t *)&g_adc_buf[0],
                   SES_STREAM_HALF_PAIRS * SES_ADC_CHANNELS * 2u);
    }
    if ((g_sonar.stream_ready & 0x02u) != 0u) {
        g_sonar.stream_ready &= (uint8_t)~0x02u;
        proto_send(SES_RSP_STREAM_CHUNK,
                   (const uint8_t *)&g_adc_buf[SES_STREAM_HALF_PAIRS * SES_ADC_CHANNELS],
                   SES_STREAM_HALF_PAIRS * SES_ADC_CHANNELS * 2u);
    }
}
