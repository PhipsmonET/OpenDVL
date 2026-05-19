# Mappatura dei Pin dello Shield (STM32H7 Nucleo Morpho)

Questo documento definisce l'assegnazione esatta dei pin tra la scheda madre STM32H7 Nucleo (tramite i connettori ST Morpho) e lo shield custom OpenDVL HAT contenente i chip di condizionamento analogico (AD600JNZ, LTC1144), il driver di gate (MAX4427CPA+) e l'ADC duale parallelo AD9226.

L'assegnazione è ottimizzata per consentire la lettura diretta in parallelo a 12-bit tramite registri GPIO interi associati al DMA ad alta velocità, e la generazione dei burst di timing a 1 MHz.

---

## Connessioni Principali dello Shield

### 1. Interfaccia AD9226 Duale (2 Canali paralleli a 12-bit)
Per acquisire a 10 Msps o 20 Msps senza impegnare la CPU, le 12 linee parallele di ciascun canale dell'AD9226 sono collegate a blocchi completi contigui di porte GPIO dell'STM32H7. Questo permette al DMA di leggere l'intero registro GPIOx->IDR in un unico ciclo di bus.

- PORTD (PD0 - PD11): Bus parallelo 12-bit Canale A (Trasduttore 1)
- PORTE (PE0 - PE11): Bus parallelo 12-bit Canale B (Trasduttore 2)
- PA8 (MCO1 o TIM8_CH1): Segnale di clock per entrambi gli ADC AD9226 (10-20 MHz)

### 2. Trasmissione (TX Burst - 1 MHz) & Gate Driver MAX4427CPA+
Il driver di gate MAX4427CPA+ accetta segnali di ingresso TTL/CMOS ed eccita i MOSFET di potenza. I segnali sono generati dal timer avanzato TIM1.

- PE9 (TIM1_CH1): Ingresso A (INA) - Comando TX 1 MHz per Canale 1 (Trasduttore 1)
- PE11 (TIM1_CH2): Ingresso B (INB) - Comando TX 1 MHz per Canale 2 (Trasduttore 2)

Nota: Le uscite del MAX4427 (OUTA - Pin 7 e OUTB - Pin 5) sono collegate direttamente ai gate dei MOSFET di potenza che alimentano i trasduttori piezoelettrici.

### 3. Controllo Guadagno Variabile AD600JNZ (TVG)
L'amplificatore AD600JNZ richiede una tensione analogica di controllo (V_G, da -0.625 V a +0.625 V) per variare il guadagno da 0 a 40 dB. Usiamo i due DAC interni a 12-bit della MCU.

- PA4 (DAC1_OUT1): Tensione TVG per canale RX 1 (Trasduttore 1)
- PA5 (DAC1_OUT2): Tensione TVG per canale RX 2 (Trasduttore 2)

### 4. Alimentazione & Controllo Inverter LTC1144CN8
L'inverter capacitivo LTC1144 inverte la tensione positiva analogica per generare il ramo a -5 V per l'AD600JNZ.
- V+ (Pin 8): Collegato alla linea a +5 V analogica dello shield.
- SHDN (Pin 3): Collegato a PC4. Impostando PC4 a HIGH, l'inverter LTC1144 si spegne (Shutdown mode) azzerando il consumo di corrente e le commutazioni capacitive durante i periodi di stand-by tra i pings acustici.

### 5. Sensori Ausiliari & Comunicazione
- I2C1 (PB6/PB7 o PB8/PB9): Collegato a un sensore di temperatura sottomarino esterno (es. TSYS01) e a un sensore di pressione (es. MS5837) tramite passacavo stagno, essenziali per la calibrazione in tempo reale della velocità del suono.
- USART1 (PA9/PA10): Porta seriale di telemetria (collegata a un transceiver RS485 come il MAX3485 sullo shield) per trasmettere i dati Vx/Vz al computer di bordo del ROV.

---

## Configurazione DMA e Trigger per l'Acquisizione
Per campionare in modo sincrono le due porte parallelamente:
1. Configurazione TIM2: Impostato in modalità timing interna con frequenza di overflow pari a 10 MHz (o 20 MHz).
2. DMA Request: L'overflow del TIM2 genera una richiesta di trasferimento DMA.
3. Canali DMA:
   - DMA1 Stream 0: Trasferisce i dati da GPIOD->IDR a un buffer RAM di destinazione (Ch A).
   - DMA1 Stream 1: Trasferisce i dati da GPIOE->IDR a un secondo buffer RAM di destinazione (Ch B).
4. Poiché entrambi i canali DMA sono sincronizzati sullo stesso trigger hardware (TIM2), le misure dei due canali sono perfettamente allineate temporalmente a livello di singolo ciclo di clock del timer.
