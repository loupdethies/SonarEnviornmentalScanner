/**
 ******************************************************************************
 * @file    proto.h
 * @brief   Binaeres Host-Protokoll ueber USB-CDC (VCP)
 *
 * Frame Geraet -> Host:
 *   Offset 0: 0xA5   Magic
 *   Offset 1: 0x5A   Magic
 *   Offset 2: u8     Frame-Typ (SES_RSP_*)
 *   Offset 3: u32 LE Payload-Laenge n
 *   Offset 7: n Byte Payload
 *
 * Befehle Host -> Geraet: 1 Befehlsbyte + Little-Endian-Parameter
 * (siehe ses_config.h).
 ******************************************************************************
 */
#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>
#include "ses_config.h"

void proto_init(void);

/* Von usbd_cdc_if.c (CDC_Receive_FS) aufzurufen */
void proto_rx(const uint8_t *data, uint32_t len);

/* Im Hauptloop zyklisch aufrufen: Befehle ausfuehren, Daten senden */
void proto_task(void);

/* Frame senden (blockierend, segmentiert intern) */
void proto_send(uint8_t type, const uint8_t *payload, uint32_t len);
void proto_send2(uint8_t type, const uint8_t *hdr, uint32_t hdr_len,
                 const uint8_t *payload, uint32_t len);

#endif /* PROTO_H */
