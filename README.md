# SonarEnvironmentalScanner (SES)

**Modularer USB-betriebener Sonar-Scanner zur Umweltkartierung und Tiererkennung**

## 🧭 Projektübersicht

Das **SonarEnvironmentalScanner**-Projekt ist ein edukatives und experimentelles System zur akustischen Kartierung von Außenumgebungen wie Gärten. Ziel ist es, mithilfe von Ultraschallwellen die Umgebung zu erfassen, Objekte zu identifizieren (z. B. Bäume, Blumen, Tiere) und akustische Signaturen von Tieren wie Vögeln oder Fledermäusen aufzunehmen.

Das System ist modular, portabel und soll per USB mit einem Computer verbunden werden. Die Signalverarbeitung erfolgt zunächst extern am PC, perspektivisch auch auf dem Mikrocontroller selbst.

---

## ⚙️ Hauptfunktionen

- 🌿 Umweltkartierung per Ultraschall
- 🐦 Tiererkennung durch akustische Analyse
- 🔌 USB-C-betriebene Stromversorgung
- 💻 Datenübertragung an PC/Laptop zur Signalverarbeitung
- 🔄 Modularer Aufbau mit Erweiterungsmöglichkeiten
- 🧪 Zukünftige Integration von Umwelt- und GPS-Sensoren

---

## 📦 Systemarchitektur (Blockdiagramm)

```
USB-C
  │
Power Management
  │
STM32F4 MCU ───── USB3.1 FIFO (FT601)
  │       │
ADC     DAC
  │       │
Mic     Transducer (40–80 kHz)
```

---

## 🔩 Verwendete Hauptkomponenten

### 🔌 USB-Verbindung und Power Management

- **USB-C Buchse:** GSB3C3133DS2HRCT
- **USB Power Controller:** STUSB4500QTR
- **Power Protection Diode:** TPD3S04
- **Buck Converter:** LMR33630ADDAR
- **Linear Regler:** LT1763CS8-2.5

### 🧠 Mikrocontroller

- **STM32F411RET6** (32-bit MCU, 512 KB Flash)
- **USB zu FIFO Bridge:** FT601Q-B-T

### 🔊 Audio-Komponenten

- **Sender:** MA40S4S (Ultraschallsender 40 kHz)
- **Mikrofon:** SPU0410LR5H-QB (MEMS Mikrofon -38 dB)

---

## 📐 Gehäuse & Formfaktor

- Format: zwischen DIN A5 und DIN A4
- Outdoor-fähig (ggf. wettergeschützt)
- Mobil einsetzbar mit Möglichkeit zur Mehrpunktvermessung

---

## 🔮 Geplante Erweiterungen

- 🌡️ Temperatur-, Luftfeuchte- und Drucksensoren
- 📍 GPS-Modul zur Positionsbestimmung
- 📶 Bluetooth oder Wi-Fi Unterstützung
- 🎛️ Onboard Signalverarbeitung (z. B. FFT)

---

## 💻 Software & Entwicklung

- ⚡ Firmware in C für STM32 (mit STM32CubeIDE)
- 🔬 Signalverarbeitung & Analyse in Python oder C auf dem PC
- 📊 Visualisierung von Messdaten (z. B. als 2D/3D-Karten)

---

## 🚀 Erste Schritte

1. **Repository klonen:**
   ```bash
   git clone https://github.com/dein-benutzername/SonarEnvironmentalScanner.git
   cd SonarEnvironmentalScanner
   ```

2. **PCB-Design öffnen:** mit [KiCad 8.0](https://kicad.org)

3. **Firmware entwickeln:** mit STM32CubeIDE

4. **Daten erfassen & analysieren:** über PC-Schnittstelle

---

## 🛠️ Tools

- **PCB Design:** KiCad 8.0
- **Simulation:** LTspice
- **Firmware:** C (STM32CubeMX & CubeIDE)
- **Analyse/Visualisierung:** Python oder C

---

## 📚 Dokumentation

Weitere Details findest du in der Konzeptdatei [`SonarEnvironmentalScanner_Concept_V1_2.pdf`](./SonarEnvironmentalScanner_Concept_V1_2.pdf)

---

## 🤝 Beitrag leisten

Pull Requests und Issues sind willkommen – gerne Feedback, Ideen oder konkrete Beiträge zur Schaltung, Firmware oder Signalverarbeitung!

---

## 📝 Lizenz

Dieses Projekt ist derzeit als Lernprojekt ausgelegt und steht unter einer Creative Commons BY-NC Lizenz. Eine finale Lizenzierung erfolgt bei Veröffentlichung der ersten Version.
