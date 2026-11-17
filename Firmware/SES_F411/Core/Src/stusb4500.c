/**
 ******************************************************************************
 * @file    stusb4500.c
 * @brief   Minimaltreiber STUSB4500 USB-PD-Sink-Controller
 ******************************************************************************
 */
#include "stusb4500.h"

static I2C_HandleTypeDef *s_hi2c;

static HAL_StatusTypeDef rd(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(s_hi2c, STUSB4500_I2C_ADDR, reg,
                            I2C_MEMADD_SIZE_8BIT, buf, len, 100u);
}

HAL_StatusTypeDef stusb4500_init(I2C_HandleTypeDef *hi2c)
{
    uint8_t id = 0u;
    s_hi2c = hi2c;

    if (rd(STUSB4500_REG_DEVICE_ID, &id, 1u) != HAL_OK) {
        return HAL_ERROR;
    }
    /* Device-ID laut Datenblatt: 0x21 (Cut 1.0) bzw. 0x25 (Cut 1.3) */
    return ((id == 0x21u) || (id == 0x25u)) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef stusb4500_read_status(stusb4500_status_t *st)
{
    uint8_t b   = 0u;
    uint8_t rdo[4];

    if (st == NULL) {
        return HAL_ERROR;
    }

    if (rd(STUSB4500_REG_PORT_STATUS_1, &b, 1u) != HAL_OK) {
        return HAL_ERROR;
    }
    st->attached = ((b & 0x01u) != 0u);          /* ATTACH-Bit               */

    (void)rd(STUSB4500_REG_DEVICE_ID, &st->device_id, 1u);

    if (rd(STUSB4500_REG_RDO_STATUS, rdo, 4u) != HAL_OK) {
        return HAL_ERROR;
    }
    st->rdo_raw = ((uint32_t)rdo[3] << 24) | ((uint32_t)rdo[2] << 16) |
                  ((uint32_t)rdo[1] << 8)  |  (uint32_t)rdo[0];

    /* Fixed-Supply-RDO: Bits 9..0 = MaxCurrent, 19..10 = OpCurrent (10 mA)  */
    st->max_current_ma = (uint16_t)((st->rdo_raw & 0x3FFu) * 10u);
    st->current_ma     = (uint16_t)(((st->rdo_raw >> 10) & 0x3FFu) * 10u);

    return HAL_OK;
}
