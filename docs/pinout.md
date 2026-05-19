# 📌 Mappatura dei Pin (STM32H7 - LQFP100)

Questo documento definisce l'assegnazione provvisoria dei pin del microcontrollore **STM32H743VIT6** per soddisfare i requisiti del Doppler Velocity Log (OpenDVL). 

L'assegnazione è ottimizzata per evitare conflitti di DMA e garantire l'accesso alle funzioni hardware avanzate (come i timer complementari e le linee SPI dedicate).

---

## 📋 Tabella Riassuntiva dei Pin

| Pin (LQFP100) | Funzione Principale | Tipo | Descrizione / Collegamento AFE o Periferiche |
| :--- | :--- | :--- | :--- |
| **PA0** | `ADC1_INP16` | Analog In | Canale di Ricezione Acustica 1 (Eco Trasduttore 1) |
| **PA1** | `ADC1_INP17` | Analog In | Canale di Ricezione Acustica 2 (Eco Trasduttore 2) |
| **PA2** | `ADC2_INP14` | Analog In | Canale di Ricezione Acustica 3 (Eco Trasduttore 3) |
| **PA3** | `ADC2_INP15` | Analog In | Canale di Ricezione Acustica 4 (Eco Trasduttore 4) |
| **PA4** | `DAC1_OUT1` | Analog Out | Regolazione Tensione Boost TX (20V-100V) |
| **PA5** | `SPI1_SCK` | Output | Bus SPI1 - Controllo Amplificatori a Guadagno Programmabile (PGA) |
| **PA6** | `SPI1_MISO` | Input | Bus SPI1 - Dati dai registri PGA |
| **PA7** | `SPI1_MOSI` | Output | Bus SPI1 - Comandi di configurazione PGA / AGC |
| **PA9** | `USART1_TX` | Output | Telemetria seriale principale ROV/AUV (RS232/RS485) |
| **PA10** | `USART1_RX` | Input | Ricezione comandi seriali da ROV/AUV |
| **PA11** | `USB_OTG_FS_DM` | Bidir | Porta USB di servizio (diagnostica / bootloader) |
| **PA12** | `USB_OTG_FS_DP` | Bidir | Porta USB di servizio (diagnostica / bootloader) |
| **PA13** | `SYS_JTMS-SWDIO` | Debug | Interfaccia di programmazione e debug SWD (ST-Link / J-Link) |
| **PA14** | `SYS_JTCK-SWCLK` | Debug | Interfaccia di programmazione e debug SWD (ST-Link / J-Link) |
| | | | |
| **PB0** | `GPIO_Output` | Digital Out | Controllo T/R Switch (Abilita protezione AFE durante TX) |
| **PB1** | `GPIO_Output` | Digital Out | PGA Chip Select (Canale 1 & 2) |
| **PB2** | `GPIO_Output` | Digital Out | PGA Chip Select (Canale 3 & 4) |
| **PB6** | `I2C1_SCL` | OD Output | Bus I2C1 - Sensore di Temperatura & Pressione Esterna |
| **PB7** | `I2C1_SDA` | OD Bidir | Bus I2C1 - Sensore di Temperatura & Pressione Esterna |
| | | | |
| **PC0** | `ADC3_INP10` | Analog In | Sensore di Temperatura interno per calibrazione di deriva |
| **PC1** | `ADC3_INP11` | Analog In | Monitoraggio Tensione di Batteria (V_Batt / Scale) |
| **PC2** | `ADC3_INP12` | Analog In | Monitoraggio Tensione di Boost TX (HV Sense) |
| | | | |
| **PE9** | `TIM1_CH1` | PWM Out | TX Burst - Segnale di eccitazione Trasduttore 1 |
| **PE11** | `TIM1_CH2` | PWM Out | TX Burst - Segnale di eccitazione Trasduttore 2 |
| **PE13** | `TIM1_CH3` | PWM Out | TX Burst - Segnale di eccitazione Trasduttore 3 |
| **PE14** | `TIM1_CH4` | PWM Out | TX Burst - Segnale di eccitazione Trasduttore 4 |

---

## ⚡ Considerazioni di Progettazione PCB e Firmware

### 1. Campionamento ADC Parallelo
- **Configurazione Multi-ADC**: L'STM32H7 possiede 3 moduli ADC indipendenti. Per garantire che la misura Doppler sia sincrona tra i diversi fasci acustici (evitando sfasamenti temporali introdotti dal multiplexing), i canali 1 & 2 sono assegnati ad **ADC1**, mentre i canali 3 & 4 sono assegnati ad **ADC2**.
- I due ADC possono essere configurati in **Dual Mode (Interleaved or Regular Simultaneous)** attivato da un timer hardware (es. `TIM2` o `TIM3`) a frequenza costante (es. $2\text{ MHz}$).

### 2. Generazione impulsi di trasmissione (TX Pings)
- Utilizziamo il timer avanzato **TIM1** che supporta la generazione di PWM con canali complementari, tempi di morto (dead-time insertion) ed è in grado di operare in modalità "one-pulse mode" per generare esattamente il numero desiderato di cicli d'onda acustica (es. 10 cicli a $300\text{ kHz}$) su tutti e 4 i trasduttori, sia in fase che sfasati se si sceglie di campionare in tempi differiti per minimizzare le interferenze acustiche tra i canali.

### 3. Bus di Comunicazione e Diagnostica
- **RS485 Half-Duplex**: La telemetria seriale (PA9/PA10) deve essere abbinata a un chip ricetrasmettitore RS485 (es. `MAX3485`) con un pin aggiuntivo di controllo direzione (DE/RE, ad esempio su **PB5**) gestito automaticamente dall'hardware dell'USART dell'STM32H7 (driver enable feature).
- **USB di Servizio**: PA11/PA12 sono dedicati ad una porta USB-C protetta da TVS per configurazione sul campo, download di log acustici non elaborati e aggiornamento firmware facilitato.
