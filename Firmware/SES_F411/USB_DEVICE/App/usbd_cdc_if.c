/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usbd_cdc_if.c
 * @brief   USB-CDC-Interface - bindet das SES-Host-Protokoll an
 *
 * Diese Datei ersetzt die von CubeMX generierte usbd_cdc_if.c. Die
 * USER-CODE-Bloecke enthalten die Anbindung an proto.c (proto_rx).
 ******************************************************************************
 */
/* USER CODE END Header */

#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "proto.h"
/* USER CODE END INCLUDE */

#define APP_RX_DATA_SIZE  512
#define APP_TX_DATA_SIZE  512

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS
};

static int8_t CDC_Init_FS(void)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    return (USBD_OK);
}

static int8_t CDC_DeInit_FS(void)
{
    return (USBD_OK);
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    /* Line-Coding etc. wird nicht benoetigt (reines Binaerprotokoll),
     * Standardantworten genuegen. */
    UNUSED(cmd);
    UNUSED(pbuf);
    UNUSED(length);
    return (USBD_OK);
}

/**
 * @brief Empfangene Daten an den Protokollparser weiterreichen und den
 *        naechsten OUT-Transfer scharfschalten.
 */
static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
    /* USER CODE BEGIN 6 */
    proto_rx(Buf, *Len);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return (USBD_OK);
    /* USER CODE END 6 */
}

/**
 * @brief Daten an den Host senden.
 * @retval USBD_OK | USBD_BUSY | USBD_FAIL
 */
uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len)
{
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 7 */
    USBD_CDC_HandleTypeDef *hcdc =
        (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
    if (hcdc == NULL) {
        return USBD_FAIL;
    }
    if (hcdc->TxState != 0) {
        return USBD_BUSY;
    }
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    /* USER CODE END 7 */
    return result;
}
