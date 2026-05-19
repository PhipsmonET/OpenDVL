# ☤ Firmware di OpenDVL (STM32H7 - C/C++)

Questo modulo racchiude il codice sorgente, l'architettura software e il sistema di build del firmware per la scheda di controllo dell'OpenDVL, basato su microcontrollore **STM32H7** (ARM Cortex-M7).

---

## 📐 Architettura del Software

Il firmware è progettato con un approccio modulare e multi-thread per garantire il determinismo temporale durante le fasi di trasmissione acustica e acquisizione ad alta velocità.

```mermaid
graph TD
    subgraph DRIVER LAYER (HAL/LL)
        TIM[Timers PWM TX]
        ADC[ADC DMA RX]
        UART[UART / SPI / I2C]
        ETH[Ethernet MAC]
    end

    subgraph SYSTEM SERVICES
        OS[FreeRTOS Kernel]
        CMSIS[CMSIS-DSP Library]
    end

    subgraph APPLICATION MODULES
        TX[Acoustic Transmitter]
        RX[Acoustic Receiver]
        DSP[DSP Engine]
        CAL[Speed of Sound Calibrator]
        NAV[Navigation Engine Janus Matrix]
        TEL[Telemetry RS485/Ethernet]
    end

    TIM --> TX
    ADC --> RX
    RX -->|Buffer Acustico| DSP
    CMSIS --> DSP
    CAL --> NAV
    DSP -->|Beam Velocities| NAV
    NAV -->|Vector Velocity| TEL
    TEL --> UART
    TEL --> ETH
    OS -->|Task Scheduling & Sync| APPLICATION_MODULES
```

---

## ⚡ Pipeline di Processing Acustico (DSP Engine)

Il calcolo della velocità Doppler si basa sulla misura dello slittamento di frequenza dell'eco ricevuta su ciascuno dei 4 canali risonanti dei trasduttori. Il flusso di elaborazione per ciascun *ping* segue questi passaggi:

1. **Trasmissione (Pulse Generation)**:
   - Viene generato un *burst* acustico della durata di $2 - 10\text{ ms}$.
   - Tipologia di impulso: **CW (Continuous Wave)** a singola frequenza fissa (es. $300\text{ kHz}$) oppure **FM (Frequency Modulation / Chirp)** con sweep lineare di frequenza (es. $280\text{ kHz} - 320\text{ kHz}$) per consentire la compressione dell'impulso (aumentando la risoluzione spaziale e la tolleranza al rumore).
2. **Finestratura e Acquisizione**:
   - Dopo un tempo di blanking (ritardo necessario affinché cessi la risonanza diretta del trasmettitore, evitando la misura del "cross-talk"), l'ADC a 16-bit della MCU acquisisce in modo sincrono tramite DMA i segnali dai 4 canali AFE.
   - Il campionamento avviene tipicamente a $2\text{ Msps}$ per canale.
3. **Filtrazione Digitale e Demodulazione IQ**:
   - I dati acquisiti passano attraverso un filtro passabanda digitale FIR o IIR implementato con le funzioni ottimizzate della libreria `CMSIS-DSP`.
   - Il segnale viene demodulato in fase e quadratura (I/Q) per traslare la frequenza centrale in banda base, riducendo drasticamente il numero di campioni da processare.
4. **Algoritmo di Stima della Frequenza**:
   - **Metodo Spettrale (FFT)**: Calcolo della FFT a 1024 o 2048 punti su finestre temporali scorrevoli dell'eco per individuare il picco spettrale che corrisponde allo spostamento Doppler.
   - **Autocorrelazione (Algoritmo di Rummler)**: Calcolo dello sfasamento medio tra campioni consecutivi del segnale IQ. È estremamente efficiente dal punto di vista computazionale ed è ampiamente utilizzato nei sonar industriali.
   - **Cross-Correlazione (per impulsi Chirp/FM)**: Correlazione del segnale ricevuto con la replica dell'impulso trasmesso. Questo metodo (detto *Matched Filter*) permette una precisione sub-centimetrica nella misura della velocità anche con rapporti segnale/rumore molto bassi ($SNR < 0\text{ dB}$).
5. **Compensazione della Velocità del Suon ($c$)**:
   - La velocità calcolata dipende direttamente dalla velocità di propagazione del suono nell'acqua.
   - Il firmware acquisisce in tempo reale la temperatura $T$ (tramite sensore analogico/I2C ad alta precisione) e la pressione $P$.
   - Utilizzando il valore di salinità stimato o impostato dall'utente ($S$), viene calcolata la velocità del suono tramite la formula di **Clay-Medwin**:
     $$c(T, S, z) = 1449.2 + 4.6T - 0.055T^2 + 0.00029T^3 + (1.34 - 0.01T)(S - 35) + 0.016z$$
     *(dove $z$ è la profondità derivata dal sensore di pressione)*.
6. **Trasformazione Geometrica (Janus Matrix)**:
   - Le velocità misurate lungo i 4 assi dei trasduttori ($b_1, b_2, b_3, b_4$) vengono convertite nel sistema di coordinate del veicolo ($V_x, V_y, V_z$ - avanti/indietro, destra/sinistra, salita/discesa) tramite la matrice di trasformazione geometrica basata sull'inclinazione $\alpha = 30^{\circ}$:
     $$V_x = \frac{c}{4\sin\alpha} \cdot \frac{\Delta f_1 - \Delta f_2}{f_0}$$
     $$V_y = \frac{c}{4\sin\alpha} \cdot \frac{\Delta f_3 - \Delta f_4}{f_0}$$
     $$V_z = \frac{c}{4\cos\alpha} \cdot \frac{\Delta f_1 + \Delta f_2 + \Delta f_3 + \Delta f_4}{f_0}$$

---

## 📂 Struttura della Cartella `firmware/`

- `CMakeLists.txt`: Configurazione di build principale.
- `toolchain-arm.cmake`: File di configurazione per la toolchain cross-compiler `arm-none-eabi`.
- `Core/`:
  - `Src/main.c`: Inizializzazione hardware (Clocks, GPIO, DMA, Periferiche).
  - `Src/stm32h7xx_it.c`: Gestori delle interruzioni (Interrupt Service Routines) per ADC DMA e Timer di Trigger.
- `App/`:
  - `Src/acoustic_tx.c`: Gestione della generazione del ping acustico.
  - `Src/acoustic_rx.c`: Gestione del ring buffer del DMA di ricezione.
  - `Src/dsp_engine.c`: Algoritmi FFT, Matched Filter e calcolo dello spostamento Doppler.
  - `Src/navigation.c`: Matrice Janus, compensazione velocità del suono.
  - `Src/telemetry.c`: Formattazione stringhe NMEA e protocollo binario PD0.
- `Drivers/`: Driver del produttore (STM32H7xx_HAL_Driver) e sorgenti CMSIS Core e DSP.

---

## 🛠️ Istruzioni per la Compilazione

### Requisiti
- **GCC Toolchain per ARM**: `arm-none-eabi-gcc` deve essere installato e presente nel PATH del sistema.
- **CMake**: Versione 3.20 o superiore.
- **Build Generator**: `make` (Linux/macOS/Windows via MinGW) oppure `ninja`.

### Procedura di Build (da Terminale/PowerShell)
```bash
# Entra nella cartella firmware
cd firmware

# Crea la directory di build
mkdir build
cd build

# Genera i file di build con CMake puntando alla toolchain ARM
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake -G "Unix Makefiles" ..

# Avvia la compilazione
make -j4
```

Al termine del processo, nella cartella `build/` verranno generati i file:
- `OpenDVL_Firmware.elf`: File binario contenente i simboli di debug per il programmatore (ST-Link, J-Link).
- `OpenDVL_Firmware.hex` / `OpenDVL_Firmware.bin`: Immagini pronte da flashare sul microcontrollore.
