# ⚡ Elettronica di OpenDVL (KiCad 10 Shield)

Questo modulo contiene il progetto hardware completo dello shield elettronico (HAT) per il Doppler Velocity Log, sviluppato in **KiCad 10**. La scheda è progettata come un **HAT/Shield** da innestare direttamente su una scheda di sviluppo **STM32H7 Nucleo (Motherboard)**.

Il design si concentra sull'acquisizione parallela ad altissima velocità tramite ADC esterno duale, sul controllo di guadagno variabile nel tempo (TVG) per compensare l'attenuazione acustica a 1 MHz, e sulla pulizia delle alimentazioni duali analogiche.

---

## 📐 Schema a Blocchi dell'Elettronica (HAT)

```mermaid
graph TD
    subgraph ALIMENTAZIONE (Power Shield)
        V_IN[V_Batt +12V o +5V da Nucleo] --> LDO_5V[LDO +5V Analogico]
        V_IN --> ChargePump[LTC1144 Voltage Inverter]
        ChargePump -->|Inversione Ramo Negativo| V_NEG[-5V Analogico]
    end

    subgraph TRASMISSIONE (TX - 1 MHz)
        MCU_TX[Nucleo H7 TIM1 PWM 1 MHz] --> GateDriver[MAX4427CPA+ MOSFET Driver]
        GateDriver --> MOSFET_Bridge[Half/Full Bridge MOSFET]
        MOSFET_Bridge --> Piezo[2x Trasduttori Piezo 1 MHz]
    end

    subgraph RICEZIONE (RX AFE)
        Piezo --> TR_Switch[T/R Switch Diodi Clamp]
        TR_Switch --> VGA[AD600JNZ Active Gain / TVG]
        DAC_MCU[Nucleo H7 DAC control] -->|Tensione Gain V_G| VGA
        VGA --> ADC_Ext[Dual AD9226 12-Bit Parallel ADC]
    end

    subgraph INTERFACCIA MOTHERBOARD
        ADC_Ext -->|2x 12-bit Bus Parallelo D0-D11| Nucleo_GPIO[Nucleo H7 GPIO Port D & E]
        Nucleo_GPIO -->|DMA Transfer triggered by TIM2| MCU_DSP[H7 Processing Engine]
    end
```

---

## 🛠️ Dettaglio Circuitale e Componenti Selezionati

### 1. Architettura Motherboard/HAT
- **Motherboard**: Scheda di sviluppo **STM32H7 Nucleo** (es. Nucleo-H743ZI o Nucleo-H723ZG). Questa scelta solleva il progettista dal routing ad altissima frequenza della MCU (480 MHz), del clock e delle memorie esterne, esponendo su comodi connettori Morpho tutte le linee ad alta velocità.
- **HAT / Shield**: Il nostro PCB custom a 4 strati che alloggia la parte di potenza TX (driver MAX4427, MOSFET, trasduttori), la catena di condizionamento analogico RX (T/R Switch, amplificatore AD600JNZ, alimentatore LTC1144) e l'ADC duale parallelo AD9226.

### 2. Trasmettitore ad alta velocità (TX - 1 MHz)
- **Frequenza di Lavoro**: **1 MHz** (lunghezza d'onda ridotta, ottima risoluzione spaziale del fondale e ridotte dimensioni dei trasduttori piezoelettrici, ideale per ROV subacquei).
- **Gate Driver (MAX4427CPA+)**: Doppio driver ad alta velocità in grado di erogare picchi di corrente fino a 1.5 A sulle capacità di gate dei MOSFET di potenza. È essenziale per commutare i MOSFET a 1 MHz mantenendo fronti di salita e discesa estremamente ripidi (< 20 ns), minimizzando le perdite di commutazione e massimizzando la potenza acustica del ping.
- **Stadio di Potenza**: Configurato come mezzo ponte (Half-Bridge) o ponte intero (Full-Bridge) a MOSFET complementari per pilotare in push-pull i due trasduttori acustici da 1 MHz.

### 3. Analog Front-End (AFE - RX) con Time-Varied Gain
- **T/R Switch**: Circuito di protezione a diodi veloci in antiparallelo (es. 1N4148W) per clampare la tensione di ingresso a ±0.7 V durante la trasmissione ad alta tensione (> 24 V), salvaguardando il preamplificatore.
- **Active Gain / VGA (AD600JNZ)**: Amplificatore a guadagno variabile (VGA) lineare in dB a bassissimo rumore (1.4 nV/rtHz). La presenza di due canali indipendenti è perfetta per il nostro sistema a due trasduttori. Ciascun canale offre fino a 40 dB di guadagno, accoppiabili in cascata fino a 80 dB.
  - **Time-Varied Gain (TVG)**: Il guadagno è regolato dinamicamente durante la ricezione dell'eco tramite una tensione analogica differenziale di controllo (V_G, da -0.625 V a +0.625 V per 0-40 dB). Questa tensione viene generata in tempo reale da uno dei canali DAC della MCU STM32H7, incrementandola in modo logaritmico/lineare man mano che il tempo dal ping aumenta per compensare l'assorbimento dell'acqua a 1 MHz.
- **Dual AD9226 12-Bit Parallel ADC**: Convertitore analogico-digitale duale a 12-bit ad altissima velocità (fino a 65 Msps).
  - Offre un SNR eccellente per la digitalizzazione diretta della portante a 1 MHz.
  - La MCU STM32H7 acquisisce i dati paralleli a 12-bit di ciascun canale associandoli a due porte GPIO complete (es. Port D per Ch A, Port E per Ch B), leggendo i registri di ingresso (IDR) tramite DMA circolare pilotato da un timer hardware (es. TIM2 a 10 Msps o 20 Msps).

### 4. Alimentazione Analogica Duale (LTC1144CN8#PBF)
- **LTC1144CN8#PBF**: Convertitore di tensione a condensatori commutati (Switched-Capacitor Charge Pump) ad alta efficienza.
  - Prende la tensione positiva di ingresso (+5 V o +12 V analogica) e la inverte sul pin di uscita per generare il ramo negativo corrispondente (-5 V o -12 V).
  - Questo fornisce la linea di alimentazione duale pulita e simmetrica indispensabile per il corretto funzionamento dell'amplificatore AD600JNZ e dei preamplificatori operazionali dell'AFE, eliminando la necessità di un trasformatore o di induttanze switching complesse che potrebbero accoppiare rumore EM a 1 MHz.
- **Filtrazione LDO**: Le uscite analogiche positiva e negativa vengono rifiltrate da regolatori lineari LDO a bassissimo rumore (es. LT3045 / LT3094) prima di alimentare la catena analogica di precisione.

---

## 📂 Struttura del Progetto KiCad 10
All'interno della cartella `OpenDVL_PCB/` troverai:
- `OpenDVL_PCB.kicad_pro`: File di progetto dello Shield.
- `OpenDVL_PCB.kicad_sch`: Foglio schematico organizzato gerarchicamente (Stadio di potenza TX con MAX4427, Stadio RX con AD600, Sezione ADC AD9226, Inverter LTC1144 e connettori Morpho Nucleo).
- `OpenDVL_PCB.kicad_pcb`: Sbroglio PCB a 4 strati (Stackup consigliato: Signal / GND / Power / Signal).
- `libs/`: Modelli footprints e simboli dedicati per l'AD600JNZ (DIP-16 / SOIC-16), l'LTC1144CN8 (DIP-8), il MAX4427CPA (DIP-8) e i moduli AD9226.

---

## 📐 Regole di Layout e Routing Consigliate (KiCad)
1. **Accoppiamento Parallelo ADC**: I due bus da 12-bit paralleli dell'AD9226 devono avere tracce della stessa lunghezza (length matching) per evitare disallineamenti temporali (skew) sui bit a frequenze di clock elevate. Mantieni le tracce corte e dirette verso i pin Morpho della Nucleo.
2. **Isolamento dell'Inverter LTC1144**: La pompa di carica lavora a commutazione di carica. Posiziona i condensatori di trasferimento (flying capacitors) e i condensatori di filtro il più vicino possibile all'IC e tieni le piste ad alta corrente di commutazione lontane dagli ingressi analogici sensibili dell'AD600JNZ per evitare accoppiamenti capacitivi di rumore.
3. **Schermatura AFE**: L'AD600JNZ ha un guadagno estremamente elevato. Le piste analogiche tra il T/R switch e l'ingresso dell'AD600 devono essere circondate da un piano di massa analogica continuo (AGND) e isolate fisicamente dalle piste PWM a 1 MHz del MAX4427.
