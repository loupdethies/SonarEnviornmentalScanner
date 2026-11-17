/**
 ******************************************************************************
 * @file    stusb4500.h
 * @brief   Minimaltreiber STUSB4500 USB-PD-Sink-Controller (I2C, Adr. 0x28)
 *
 * Der STUSB4500 verhandelt den PD-Kontrakt autonom aus dem NVM. Dieser
 * Treiber liest nur den Status (Attach, verhandelte Spannung/Strom) aus.
 ******************************************************************************
 */
#ifndef STUSB4500_H
#define STUSB4500_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define STUSB4500_I2C_ADDR          (0x28u << 1)   /* HAL: 8-Bit-Adresse    */

/* Registeradressen (Datenblatt STUSB4500, Kap. Register Map) */
#define STUSB4500_REG_PORT_STATUS_1     0x0Eu
#define STUSB4500_REG_RDO_STATUS        0x91u      /* RDO, 4 Byte LE        */
#define STUSB4500_REG_DEVICE_ID         0x2Fu

typedef struct {
    bool     attached;         /* USB-C-Partner angeschlossen               */
    uint8_t  device_id;
    uint32_t rdo_raw;          /* aktives Request Data Object               */
    uint16_t current_ma;       /* verhandelter Betriebsstrom                */
    uint16_t max_current_ma;
} stusb4500_status_t;

HAL_StatusTypeDef stusb4500_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef stusb4500_read_status(stusb4500_status_t *st);

#endif /* STUSB4500_H */
