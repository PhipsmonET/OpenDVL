# ⚡ Elettronica di OpenDVL (KiCad 10)

Questo modulo contiene il progetto hardware completo del Doppler Velocity Log, sviluppato in **KiCad 10**. Il design si concentra sull'ottimizzazione del rapporto segnale-rumore (SNR) dell'eco acustica ad alta frequenza e sulla robustezza dell'alimentazione in ambiente marino.

---

## 📐 Schema a Blocchi dell'Elettronica

```mermaid
graph LR
    subgraph ALIMENTAZIONE
        V_IN[V_Batt 12V-24V] --> D_PROT[Protezione Polarità / TVS]
        D_PROT --> DC_HV[Boost Converter 20V-100V Tx]
        D_PROT --> DC_5V[Buck Converter 5V]
        DC_5V --> LDO_3V3[LDO Analog 3.3V]
        DC_5V --> LDO_1V8[LDO Digital 1.8V/3.3V MCU]
    end

    subgraph TRASMISSIONE (TX)
        MCU_TX[STM32H7 PWM/GPIO] --> TX_DRV[Gate Driver]
        TX_DRV --> TX_PULSER[High-Voltage H-Bridge / Pulser]
        DC_HV --> TX_PULSER
        TX_PULSER --> Piezo[Trasduttori Piezoelettrici]
    end

    subgraph RICEZIONE (RX)
        Piezo --> TR_SWITCH[T/R Switch Protezione HV]
        TR_SWITCH --> RX_LNA[Low Noise Amplifier]
        RX_LNA --> BPF[Filtro Passa-Banda Attivo]
        BPF --> PGA[Programmable Gain Amplifier]
        PGA --> ADC[MCU ADC / ADC Esterno SPI]
    end
```

---

## 🛠️ Dettaglio Circuitale e Scelta dei Componenti

### 1. Microcontrollore (MCU)
- **Modello Consigliato**: `STM32H743VIT6` o `STM32H753VIT6` (LQFP-100).
- **Perché**: CPU Cortex-M7 a 480 MHz, 2MB Flash, 1MB RAM. Dispone di ADC integrati a 16-bit ad altissima velocità (fino a 3.6 Msps), supporto DMA avanzato e istruzioni DSP a ciclo singolo. Inoltre, l'interfaccia Ethernet integrata facilita la trasmissione dei dati di telemetria.

### 2. Sezione Trasmettitore (TX Pulser)
- **Descrizione**: Genera impulsi ad alta tensione alternati per eccitare i dischi piezoelettrici alla loro frequenza di risonanza (tipicamente $300\text{ kHz} - 500\text{ kHz}$).
- **Componenti chiave**:
  - Un convertitore Boost regolabile (tramite DAC o potenziometro digitale) da $20\text{ V}$ a $100\text{ V}$ per regolare la potenza acustica trasmessa in base alla profondità del fondale.
  - Driver H-Bridge integrati (es. `MD1211` + `TC6320` o MOSFET complementari ad alta velocità) in grado di erogare picchi di corrente significativi sui carichi capacitivi dei trasduttori piezoelettrici.

### 3. Analog Front-End (AFE - RX)
Poiché il segnale di eco riflesso dal fondale può essere dell'ordine dei microvolt ($\mu\text{V}$), la catena di ricezione è critica:
- **T/R Switch (Transmit/Receive)**: Un circuito passivo basato su diodi di clamp veloci a basso leakage (es. `1N4148W`) e resistori di limitazione per bloccare l'impulso ad alta tensione (fino a $100\text{ V}$) della trasmissione, impedendo che distrugga i preamplificatori di ricezione sensibili.
- **Preamplificatore (LNA)**: Amplificatore a bassissimo rumore (es. `OPA837` o `AD8099`) con guadagno fisso di $+20\text{ dB}$.
- **Filtro Passa-Banda (BPF)**: Filtro attivo a retroazione multipla (MFB) centrato sulla frequenza dei trasduttori (es. $300\text{ kHz}$ o $500\text{ kHz}$) con un fattore di merito $Q \approx 5-10$ per eliminare il rumore fuori banda dei motori del ROV.
- **Amplificatore a Guadagno Programmabile (PGA)**: `LTC6910` o `AD8251`, controllato via SPI/GPIO dalla MCU per implementare il controllo automatico del guadagno (**AGC**) o il guadagno variabile nel tempo (**TVG - Time Varied Gain**), compensando l'attenuazione geometrica e di assorbimento dell'acqua man mano che l'eco ritorna da distanze maggiori.

### 4. Alimentazione
- Per evitare il rumore accoppiato sui canali analogici ad alta sensibilità, le alimentazioni dell'AFE devono essere separate da quelle digitali.
- Utilizzare LDO a bassissimo rumore ed elevata PSRR (es. `LT3045` per il ramo positivo, `LT3094` per il ramo negativo se necessario, o analoghi ad alte prestazioni) per alimentare gli operazionali dell'AFE.

---

## 📂 Struttura del Progetto KiCad 10
All'interno della cartella `OpenDVL_PCB/` troverai:
- `OpenDVL_PCB.kicad_pro`: File di progetto KiCad.
- `OpenDVL_PCB.kicad_sch`: Schema elettrico principale (strutturato con fogli gerarchici per separare MCU, TX, AFE, Alimentazione).
- `OpenDVL_PCB.kicad_pcb`: Layout del circuito stampato (consigliato PCB a 4 o 6 strati con piani di massa analogici e digitali separati e uniti in un singolo punto di stella).
- `libs/`: Librerie locali.
  - `symbols/`: Simboli dei componenti non standard (es. connettori subacquei, trasduttori, pulser).
  - `footprints/`: Impronte dei componenti custom.
  - `3d/`: Modelli 3D (STEP/WRL) per la verifica degli ingombri meccanici all'interno del cilindro Fusion 360.

---

## 📐 Regole di Layout Consigliate (KiCad)
1. **Piani di Massa (GND)**: Separare rigorosamente il piano `AGND` (Analog Ground, sotto l'AFE) dal piano `DGND` (Digital Ground, sotto la MCU e i regolatori switching). Collegarli tramite una perlina di ferrite (ferrite bead) o un ponticello a punto singolo (net-tie) in prossimità dell'ADC.
2. **Linee dei Trasduttori**: Tracciare le piste dal trasmettitore ad alta tensione ai trasduttori con uno spessore generoso e con un isolamento adeguato (creepage/clearance minima di $1\text{ mm}$ tra le linee HV e quelle a bassa tensione).
3. **Piste di Ricezione**: Mantenere le connessioni tra il T/R switch, l'LNA e il primo filtro il più corte possibile per ridurre al minimo l'induttanza parassita e l'accoppiamento di rumore EM. Schermare queste tracce con tracce di guardia collegate a GND.
