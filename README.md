# 🌊 OpenDVL: Open Source Doppler Velocity Log

[![Project Status: Active](https://img.shields.io/badge/Project%20Status-Active-emerald.svg?style=for-the-badge)]()
[![Hardware: KiCad 10](https://img.shields.io/badge/Hardware-KiCad%20v10.0-blue.svg?style=for-the-badge)]()
[![Firmware: STM32H7](https://img.shields.io/badge/Firmware-STM32H7%20(ARM%20Cortex--M7)-red.svg?style=for-the-badge)]()
[![Mechanical: Fusion 360](https://img.shields.io/badge/Mechanical-Autodesk%20Fusion%20360-orange.svg?style=for-the-badge)]()

Benvenuto in **OpenDVL**, un progetto open-source e ad alte prestazioni per la realizzazione di un **Doppler Velocity Log (DVL)** dedicato alla navigazione di veicoli subacquei autonomi (AUV) e filoguidati (ROV).

Il DVL misura la velocità tridimensionale del veicolo rispetto al fondale marino trasmettendo impulsi acustici (pings) attraverso un array di 4 trasduttori piezoelettrici disposti in configurazione **Janus** (orientati a 30° rispetto alla verticale) e calcolando lo spostamento di frequenza (effetto Doppler) dell'eco di ritorno.

---

## 🛠️ Architettura di Sistema

Il progetto si articola su tre pilastri ingegneristici principali, integrati in modo sinergico:

```mermaid
graph TD
    subgraph MECCANICA (Fusion 360)
        A[Transducer Janus Mount] -->|Alloggiamento| B[Chassis Cilindrico IP68]
        C[Sezione Doppia Tenuta O-Ring] -->|Impermeabilizzazione| B
        D[Wet-Mateable Connectors] -->|Passacavi| B
    end

    subgraph ELETTRONICA (KiCad 10)
        E[Alimentazione & Filtri LDO] --> F[MCU STM32H7]
        G[Analog Front-End AFE] -->|Campionamento ADC| F
        H[Transmitter High-Voltage Pulser] -->|Eccitazione Piezo| G
        F -->|Segnali di Controllo & DAC| H
        F -->|RS232 / RS485 / Ethernet| I[Interfaccia ROV/AUV]
    end

    subgraph FIRMWARE (STM32H7 C/C++)
        F --> J[Driver Transducer Control & Timing]
        F --> K[Acoustic Processing Engine FFT/Cross-Correlation]
        F --> L[Speed of Sound Compensation Temp/Salinity]
        F --> M[Serial/Ethernet Protocol Handler PD0/NMEA]
    end
```

---

## 📁 Struttura del Repository

La struttura del progetto è organizzata per garantire la massima modularità, separando chiaramente le discipline di sviluppo:

```text
OpenDVL/
├── .gitignore               # Configurazione Git per escludere file temporanei di IDE, KiCad e CAD
├── README.md                # Questo file (Overview e Guida Generale)
│
├── docs/                    # Documentazione di sistema e specifiche tecniche
│   ├── pinout.md            # Mappatura completa dei pin della MCU STM32H7
│   └── architecture.md      # Descrizione approfondita del processing acustico
│
├── electronics/             # Progetto Elettronico (KiCad v10.0)
│   ├── README.md            # Specifiche AFE, TX Pulser, alimentazione e BOM
│   ├── OpenDVL_PCB/         # File di progetto schematico e PCB layout (.kicad_sch, .kicad_pcb)
│   └── libs/                # Librerie locali del progetto (Simboli, Footprint, Modelli 3D)
│
├── mechanics/               # Progetto Meccanico (Autodesk Fusion 360)
│   ├── README.md            # Linee guida di tenuta a pressione, tolleranze e assemblaggio
│   ├── exports/             # Modelli CAD esportati (STEP, IGES, STL per stampa 3D)
│   └── drawings/            # Tavole tecniche 2D in formato PDF/DXF
│
└── firmware/                # Firmware della MCU (STM32H7 - C/C++)
    ├── README.md            # Architettura software, DSP Pipeline, RTOS e istruzioni di build
    ├── CMakeLists.txt       # File di configurazione CMake per cross-compilazione ARM
    ├── toolchain-arm.cmake  # File di configurazione della toolchain GCC-ARM
    ├── Core/                # Codice sorgente principale (Inc, Src, Startup)
    ├── Drivers/             # HAL STM32H7, CMSIS e librerie esterne
    ├── App/                 # Logica applicativa (DSP Engine, Protocolli, Calibrazione)
    └── Middlewares/         # Librerie middleware (STM32 DSP Library, FreeRTOS)
```

---

## 📐 Specifiche dei Sotto-Sistemi

### 1. Elettronica (`/electronics`)
Progettata interamente in **KiCad 10**, la scheda di controllo si occupa di:
- **Processing Unit**: STM32H7 (Core ARM Cortex-M7 a 480 MHz, FPU a doppia precisione e istruzioni DSP hardware).
- **Trasmissione**: Generatore di impulsi ad alta tensione regolabile (20V - 100V) per pilotare i trasduttori piezoelettrici (tipicamente a 300 kHz - 1 MHz).
- **Ricezione (Analog Front-End)**: Amplificatore a guadagno programmabile (PGA), filtro passa-banda attivo di ordine elevato e circuito di protezione da sovratensione (T/R Switch) per isolare l'ADC durante la trasmissione ad alta tensione.
- **Sensori di Supporto**: Sensore di temperatura ad alta precisione (per la compensazione della velocità del suono) e sensore di pressione/profondità opzionale.

### 2. Meccanica (`/mechanics`)
Sviluppata in **Autodesk Fusion 360**, garantisce l'operatività in ambiente marino profondo:
- **Alloggiamento a Pressione**: Cilindro in alluminio anodizzato duro 6061-T6 o POM-C, calcolato per resistere a pressioni idrostatiche fino a 300m (30 bar) o superiori.
- **Testata Trasduttori**: Flangia Janus a 4 fori orientati a 30 gradi rispetto all'asse centrale per ospitare i trasduttori acustici immersi in resina poliuretanica acusticamente trasparente.
- **Sistemi di Tenuta**: Doppia scanalatura per O-Ring radiali e assiali calcolati secondo gli standard Parker.

### 3. Firmware (`/firmware`)
Scritto in C/C++ strutturato per **STM32H7**, implementa:
- **Timing di Precisione**: Generazione di burst di trasmissione (CW o FM/Chirp) sincroni tramite timer avanzati (HRTIM / TIM1/TIM8).
- **Campionamento DMA**: Acquisizione ADC multicanale parallela sincronizzata ad alta velocità gestita tramite DMA circolare per non sovraccaricare la CPU.
- **Algoritmo DSP**:
  - Trasformata Rapida di Fourier (FFT) per la stima del picco spettrale.
  - Autocorrelazione o Cross-Correlazione del segnale ricevuto rispetto alla replica trasmessa per ottenere una risoluzione sub-binaria dello spostamento Doppler.
- **Compensazione**: Ricalcolo in tempo reale della velocità del suono in acqua usando l'equazione di *Clay-Medwin* o *Del Grosso* in base a temperatura e salinità.
- **Comunicazione**: Pacchetti dati in formato standard **NMEA** (es. `$VDVHW`, `$VDVWT`) e protocollo binario **PD0** per la massima compatibilità con i sistemi di navigazione ROS / ArduSub.

---

## 🚀 Come Iniziare

### 🛠️ Configurazione Ambiente di Sviluppo Firmware
Il firmware è configurato per essere compilato tramite **CMake** e la toolchain **GCC ARM Embedded**.

1. **Requisiti**:
   - `arm-none-eabi-gcc` (Toolchain di compilazione)
   - `CMake` (versione >= 3.20)
   - `Ninja` o `Make`
   - `openocd` o `ST-Link Utility` per il flashing

2. **Compilazione**:
   ```bash
   cd firmware
   mkdir build && cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

### ⚡ Apertura Schemi Elettronici (KiCad 10)
1. Installa **KiCad 10** dal sito ufficiale.
2. Apri KiCad e seleziona `File > Open Project`.
3. Naviga in `electronics/OpenDVL_PCB/` e seleziona il file `.kicad_pro`.

### 🔩 Importazione Modelli CAD (Fusion 360)
1. Nella cartella `mechanics/exports/` trovi i file master in formato `.step` dell'intero assieme.
2. Importa il file STEP all'interno del tuo workspace di Fusion 360 per visualizzare e modificare i componenti meccanici, lo spessore delle pareti e i dettagli di lavorazione al tornio/fresa CNC.

---

## 📜 Licenza
Questo progetto è rilasciato sotto licenza **MIT** (per il firmware) e **CERN OHL-W v2** (per l'hardware elettronico e meccanico). Consulta i file di licenza specifici nelle rispettive cartelle per maggiori dettagli.
