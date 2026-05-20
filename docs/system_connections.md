# Connessioni di Sistema dell'OpenDVL

Questo documento definisce l'architettura completa di connessione dell'OpenDVL, descrivendo l'interfaccia tra la scheda madre STM32H7 Nucleo, lo shield custom (HAT), i sensori bagnati e il computer di bordo del veicolo subacqueo (ROV/AUV).

---

## 1. Collegamento Interno: Nucleo-144 e Shield Custom

La connessione fisica tra la scheda di sviluppo STM32H7 Nucleo e lo shield avviene tramite accoppiamento diretto (sandwich) sfruttando i connettori Morpho femmina/maschio a doppia riga posizionati sui bordi della scheda madre.

### Allineamento Meccanico e Segnali
- I connettori Morpho (CN7, CN8, CN9, CN10) portano sullo shield tutte le linee di segnale critiche, l'alimentazione digitale a +3.3V e +5V della Nucleo, e le masse digitali (GND).
- Lo shield custom si estende sopra la scheda Nucleo, alloggiando i componenti di potenza (MAX4427CPA+), l'AFE analogico (AD600JNZ, LTC1144), l'ADC duale parallelo AD9226 (scheda figlia) e i filtri di alimentazione.

---

## 2. Interfaccia Esterna: Collegamento verso il ROV/AUV

Il DVL e un sistema autonomo racchiuso in un tubo a pressione stagno. Il collegamento con il resto del veicolo (alimentazione principale e computer di navigazione, es. Raspberry Pi o Pixhawk) avviene tramite un singolo connettore subacqueo stagno passante sul tappo posteriore (End Cap).

### Scelta del Connettore
Si consiglia un connettore standard industriale **SubConn MCBH** a 8 pin maschio da pannello (bulkhead) montato sul tappo posteriore, abbinato a un cavo ombelicale femmina stagno.

### Mappatura dei Pin del Connettore Esterno (SubConn MCBH8M)

| Pin | Segnale | Tipo | Destinazione / Descrizione |
| :--- | :--- | :--- | :--- |
| **1** | V_BATT | Power In | Alimentazione principale dal ROV (12V - 24V DC nominali) |
| **2** | GND | Power / Signal | Massa comune di alimentazione e segnale |
| **3** | RS485_A | Bidir | Linea seriale differenziale bilanciata A (seriale di telemetria) |
| **4** | RS485_B | Bidir | Linea seriale differenziale bilanciata B (seriale di telemetria) |
| **5** | TRIG_EXT | Input | Sincronizzazione esterna a 3.3V (previene mutua interferenza con altri sonar) |
| **6** | LEAK_OUT | Output | Uscita open-drain hardware di allarme infiltrazione acqua (opzionale) |
| **7** | ETH_TX+ | Output | Linea di trasmissione Ethernet differenziale + (opzionale per debug/log) |
| **8** | ETH_TX- | Output | Linea di trasmissione Ethernet differenziale - (opzionale per debug/log) |

### Perche la scelta del Bus RS485?
Al posto della RS232, la telemetria seriale principale dell'OpenDVL e basata su **RS485 Half-Duplex** (utilizzando un ricetrasmettitore come il MAX3485 sullo shield collegato alla USART1 della Nucleo). La linea differenziale bilanciata e immune al rumore elettromagnetico generato dai motori brushless e dai regolatori elettronici di velocità (ESC) del ROV, permettendo comunicazioni stabili anche su cavi ombelicali lunghi oltre 100 metri.

---

## 3. Interfaccia con la Testata Janus (Sensori Bagnati)

La flangia anteriore bagnata ospita i trasduttori piezoelettrici da 1 MHz e i sensori di misura ambientale.

### A. Connessione dei Trasduttori Piezoelettrici (1 MHz)
Dato l'elevato guadagno dell'AD600JNZ e l'alta frequenza di 1 MHz, e fondamentale proteggere le linee di ricezione dal rumore captato per via capacitiva o induttiva:
- Collegamento: Ciascun trasduttore piezoelettrico e connesso allo shield tramite un cavo coassiale schermato a bassissima perdita (es. miniatura RG178 o RG196).
- La calza schermata del coassiale e saldata alla massa analogica (AGND) dello shield solo dal lato elettronica, mentre l'anima centrale porta il segnale caldo (TX/RX). Questo garantisce una schermatura elettrostatica completa contro il rumore emesso dal MAX4427CPA+ e dai motori.

### B. Sensore di Temperatura Bagnato (Compensazione Velocita del Suono)
Per calcolare con precisione millimetrica la velocita del suono, e indispensabile un sensore di temperatura metallico esterno a contatto diretto con l'acqua (es. sonda TSYS01 o sensore a termoresistenza PT100).
- Il sensore passa attraverso la testata Janus tramite un penetratore stagno a compressione.
- I 4 conduttori (SDA, SCL, 3.3V, GND per bus I2C) sono collegati ai pin della porta I2C1 della Nucleo presenti sullo shield, protetti da diodi di protezione TVS contro le scariche elettrostatiche.

---

## 4. Sistemi di Protezione Interni

### Sensore di Infiltrazione Acqua (Leak Detector)
Un allagamento del tubo a pressione stagno distruggerebbe l'elettronica.
- Implementazione: Due piste di rame parallele ed esposte (non coperte da solder mask), distanziate di 1 mm sul bordo inferiore dello shield o collocate su una piccola basetta sul fondo del tubo.
- Funzionamento: Una pista e collegata a GND, l'altra a un pin GPIO dell'STM32H7 con resistenza di pull-up interna abilitata (es. PC5).
- In caso di infiltrazione di acqua salata, la conduttivita del liquido cortocircuita le due piste portando il pin GPIO a LOW. Il firmware rileva l'evento tramite interrupt, interrompe immediatamente la trasmissione TX ad alta tensione per salvare i circuiti ed invia una stringa di allarme critico al ROV tramite il bus RS485.
