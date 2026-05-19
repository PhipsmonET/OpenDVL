# OpenDVL: Open Source Doppler Velocity Log

Benvenuto in OpenDVL, un progetto open-source e ad alte prestazioni per la realizzazione di un Doppler Velocity Log (DVL) bidimensionale dedicato alla navigazione di piccoli veicoli subacquei autonomi (AUV) e filoguidati (ROV).

Il sistema è basato su una scheda madre STM32H7 Nucleo, abbinata a un HAT/Shield elettronico custom e a due trasduttori piezoelettrici operanti a 1 MHz inclinati a 30 gradi rispetto alla verticale. Questo permette di calcolare la velocità bidimensionale del veicolo (avanzamento e discesa/salita) misurando lo spostamento di frequenza (effetto Doppler) dell'eco di ritorno dal fondale.

---

## Architettura di Sistema

Il progetto si articola su tre pilastri ingegneristici principali, integrati in modo sinergico:

[MECCANICA (Fusion 360)]
  * Alloggiamento piezo 1 MHz in configurazione Janus 2D
  * Tubo a pressione cilindrico calcolato per tenuta stagna IP68
  * Doppia tenuta ad O-Ring assiale e radiale
  * Connettore subacqueo wet-mateable MCBH
         |
         v (Integrazione Fisica)
[ELETTRONICA (KiCad 10 Shield)]
  * Alimentazione duale analogica +5V/-5V tramite pompa LTC1144
  * Catena di pre-amplificazione e protezione con T/R Switch
  * Controllo di guadagno dinamico TVG con chip AD600JNZ
  * Digitalizzazione ad alta velocità con ADC duale parallelo AD9226 (12-bit)
         |
         v (Acquisizione parallela via GPIO D & E con DMA)
[FIRMWARE (STM32H7 C/C++ su Nucleo)]
  * Timer di trigger TIM2 per il campionamento sincrono DMA (10-20 MSPS)
  * Demodulazione digitale IQ a 1 MHz e filtraggio passa-banda FIR
  * Algoritmo spettrale di autocorrelazione (metodo di Rummler)
  * Calcolo Vx e Vz tramite compensazione Clay-Medwin e matrice Janus 2D
  * Telemetria seriale RS485 in formato stringhe NMEA

---

## Struttura del Repository

La struttura del progetto è organizzata per garantire la massima modularità, separando chiaramente le discipline di sviluppo:

OpenDVL/
├── .gitignore               # Configurazione Git per escludere file temporanei di IDE, KiCad e CAD
├── README.md                # Questo file (Overview e Guida Generale)
│
├── docs/                    # Documentazione di sistema e specifiche tecniche
│   └── pinout.md            # Mappatura dei pin Morpho della Nucleo per lo shield
│
├── electronics/             # Progetto Elettronico (KiCad v10.0 Shield)
│   ├── README.md            # Specifiche AFE (AD600), TX (MAX4427), Inverter (LTC1144), ADC (AD9226)
│   ├── OpenDVL_PCB/         # File di progetto schematico e PCB layout
│   └── libs/                # Librerie locali del progetto (Simboli, Footprint, Modelli 3D)
│
├── mechanics/               # Progetto Meccanico (Autodesk Fusion 360)
│   ├── README.md            # Linee guida di tenuta a pressione, tolleranze e alloggiamento piezo 1 MHz
│   ├── exports/             # Modelli CAD esportati (STEP, IGES, STL per stampa 3D)
│   └── drawings/            # Tavole tecniche 2D in formato PDF/DXF
│
└── firmware/                # Firmware della MCU (STM32H7 - C/C++)
    ├── README.md            # Architettura, DMA parallelo ADC, DSP Pipeline, istruzioni
    ├── CMakeLists.txt       # File di configurazione CMake per compilazione ARM GCC
    ├── toolchain-arm.cmake  # File di configurazione della toolchain ARM
    ├── Core/                # Codice sorgente principale (Inc, Src, Startup)
    ├── App/                 # Logica applicativa (DSP Engine, Protocollo NMEA, Calibrazione)
    └── Drivers/             # HAL STM32H7, CMSIS e librerie esterne

---

## Specifiche dei Sotto-Sistemi

### 1. Elettronica (/electronics)
Lo shield custom progettato in KiCad 10 include:
- Catena RX (AFE): Switch T/R per la protezione, amplificatore a guadagno variabile lineare in dB AD600JNZ (alimentato in duale +/- 5V) controllato dal DAC della MCU per implementare il TVG (Time-Varied Gain), accoppiato all'ADC duale parallelo AD9226 a 12-bit (campionato fino a 20 Msps per asse).
- Catena TX: Segnale acustico a 1 MHz pilotato dal chip gate driver MAX4427CPA+ (corrente di picco 1.5 A) per pilotare i MOSFET di potenza a commutazione rapida.
- Alimentazione: Convertitore capacitivo LTC1144 per la generazione della linea negativa a -5V a partire dai +5V stabili della scheda madre, con controllo di spegnimento software (SHDN) per risparmiare energia durante le pause tra i ping.

### 2. Meccanica (/mechanics)
Sviluppata in Autodesk Fusion 360, garantisce l'operatività in ambiente marino:
- Alloggiamento a Pressione: Cilindro stagno di precisione in POM-C o Alluminio anodizzato a spessore, calcolato per resistere alla pressione marina.
- Testata dei Trasduttori: Alloggiamento inclinato a 30 gradi per ospitare i due trasduttori acustici da 1 MHz, con colata di resina poliuretanica acusticamente trasparente per garantire la sigillatura e l'accoppiamento con l'acqua.
- Tenuta: Doppia scanalatura per O-Ring radiali e assiali calcolati secondo gli standard industriali.

### 3. Firmware (/firmware)
Scritto in C/C++ strutturato per STM32H7, implementa:
- Campionamento DMA Parallelo: Acquisizione sincrona a 12-bit paralleli per i due canali AD9226 collegati alle porte GPIO D ed E, attivata da timer hardware e gestita tramite doppio canale DMA circolare.
- Elaborazione DSP:
  - Demodulazione digitale IQ a 1 MHz per portare il segnale acustico in banda base.
  - Algoritmo di autocorrelazione spettrale (metodo di Rummler) per la stima immediata dello spostamento Doppler in frequenza.
- Compensazione e Navigazione:
  - Ricalcolo della velocità del suono nell'acqua basato sulla temperatura misurata (equazione di Clay-Medwin).
  - Matrice Janus 2D per estrarre la velocità orizzontale di avanzamento (Vx) e la velocità verticale (Vz) a partire dagli scostamenti di frequenza misurati.

---

## Come Iniziare

### Configurazione Ambiente di Sviluppo Firmware
Il firmware si compila tramite CMake e la toolchain GCC ARM Embedded.

1. Requisiti:
   - arm-none-eabi-gcc (Toolchain di compilazione)
   - CMake (versione >= 3.20)
   - Ninja o Make

2. Compilazione:
   ```bash
   cd firmware
   mkdir build && cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

### Apertura Schemi Elettronici (KiCad 10)
1. Apri KiCad 10 e seleziona File > Open Project.
2. Seleziona il file .kicad_pro situato in electronics/OpenDVL_PCB/.

### Importazione Modelli CAD (Fusion 360)
1. I file CAD master in formato .step si trovano in mechanics/exports/.
2. Importa il file nell'ambiente Fusion 360 per integrarlo nel design del tuo ROV/AUV.

---

## Licenza
Rilasciato sotto licenza MIT (per il firmware) e CERN OHL-W v2 (per l'hardware).
