# SES-Firmware (STM32F411RET6)

Firmware für den **Sonar Environmental Scanner**, Hardware-Stand
`Design_Files/V1_1` (Schaltplan SonarLandscapeScanner).

## Architektur

| Peripherie | Funktion |
|---|---|
| ADC1 IN0/IN1 (PA0/PA1) | Mikrofonkanäle links/rechts (INA333), 12 Bit |
| TIM2 → TRGO | ADC-Abtasttrigger, 200 kHz je Kanal |
| DMA2 Stream0 | ADC → RAM (normal für Einzelmessung, zirkulär für Streaming) |
| TIM1 CH1/CH1N (PA8/PB13) | 40-kHz-Burst, One-Pulse + Repetition Counter, Totzeit 250 ns |
| I2C1 (PB6/PB7) | STUSB4500 USB-PD-Controller (Adr. 0x28) |
| PC6/PC7 | STUSB4500 ALERT/ATTACH |
| PC8 | Enable LT8330 Boost (Senderversorgung) |
| USART2 (PA2/PA3) | Debug-Konsole 115200 8N1 |
| USB OTG FS (PA11/PA12) | CDC (virtueller COM-Port), Binärprotokoll |

Takt: HSE 8 MHz → PLL (M=8, N=192, P=2, Q=4) → 96 MHz SYSCLK, 48 MHz USB.

**Hinweis Abtastrate:** USB 2.0 Full Speed begrenzt kontinuierliches Streaming
auf real ~500–800 KiB/s. Einzelmessungen (Burst + Echofenster) laufen mit
200 kS/s je Kanal aus dem RAM-Puffer (max. 8192 Samplepaare = 32 KiB ≈ 41 ms
≈ 7 m Reichweite). Die im Konzept genannten 1,5 MS/s erfordern die
FT601-Variante (V2).

## Projekt aufsetzen (STM32CubeIDE)

1. `SES_F411.ioc` in STM32CubeIDE öffnen (Firmware-Paket STM32Cube FW_F4)
   und **Generate Code** ausführen. Dadurch werden HAL, CMSIS, Startup-Code,
   Linkerskript und USB-Device-Middleware erzeugt.
2. Die hier mitgelieferten Dateien liegen bereits an den richtigen Stellen
   und werden dank `/* USER CODE */`-Blöcken bei einer Neugenerierung nicht
   überschrieben:
   - `Core/Src/main.c`, `Core/Src/sonar.c`, `Core/Src/proto.c`,
     `Core/Src/stusb4500.c`, `Core/Src/stm32f4xx_hal_msp.c`,
     `Core/Src/stm32f4xx_it.c`
   - `Core/Inc/*.h`
   - `USB_DEVICE/App/usbd_cdc_if.c` (ersetzt die generierte Datei)
3. Prüfen, dass CubeMX keine abweichenden `main.c`/`usbd_cdc_if.c` erzeugt
   hat (bei Konflikt die Versionen aus diesem Repository verwenden).
4. Build & Flash per ST-LINK über den SWD-Header (J2).

## USB-Protokoll

Gerät → Host Frames: `A5 5A | Typ (1 B) | Länge (u32 LE) | Payload`

| Befehl (Host → Gerät) | Parameter | Antwort |
|---|---|---|
| `V` | – | `0x01` Version (major, minor, patch, 0) |
| `M` | u16 nPulses, u32 nPairs | `0x02` Header (u32 fs, u16 nPulses, u32 nPairs) + interleavte u16-Daten L/R |
| `A` | – | fortlaufend `0x03` Chunks (halber Puffer) |
| `S` | – | – (stoppt Messung/Streaming) |
| `B` | u8 on | – (Boost-Wandler ein/aus) |
| `G` | – | `0x04` PD-Status (attach, devID, RDO, Ströme) |
| – | – | `0x7F` Fehlercode |

Beispiel-Client: `Host/ses_client.py` (pyserial, numpy, matplotlib):

```bash
python Host/ses_client.py COM5 --measure
```

## Offene Punkte (Hardware-abhängig)

- Treiberstufe für den MA40S4S (TX_PWM/TX_PWM_N → Halbbrücke an LT8330-Rail)
  ist im Schaltplan V1_1 noch nicht ausgeführt — Burst-Signal liegt an PA8/PB13.
- STUSB4500-NVM ggf. mit STSW-STUSB002 auf gewünschte PDOs programmieren
  (Default-Sink-Profile; RESET liegt fest auf GND).
- Signalverarbeitung (Matched Filter, FFT-Tiererkennung) erfolgt hostseitig.
