# OpenDVL: Open Source Doppler Velocity Log

Benvenuto in OpenDVL, un progetto open-source e ad alte prestazioni per la realizzazione di un Doppler Velocity Log (DVL) bidimensionale dedicato alla navigazione autonoma sottomarina (GPS-denied) di piccoli veicoli autonomi (AUV) e ROV.

Il sistema e basato su una scheda madre STM32H7 Nucleo abbinata a uno shield elettronico custom realizzato con una filosofia mista THT/DIP (per semplificare il montaggio manuale e la prototipazione). Il DVL utilizza due trasduttori piezoelettrici industriali da 1 MHz (diametro 20 mm, capacita 1200 pF, impedenza 130 Ohm, resistenza strutturale alla pressione di 1.5 MPa / 150 metri) disposti in configurazione Janus 2D inclinati a 30 gradi rispetto alla verticale.

OpenDVL integra a livello nativo la compatibilita con l'ecosistema ArduPilot (ArduSub), trasmettendo le misure di velocita nel formato seriale Waterlinked-compatible JSON via bus RS485 differenziale a 115200 baud, consentendo il posizionamento autonomo e la stabilizzazione (Position Hold) del veicolo tramite il filtro di navigazione EKF3.

---

## Architettura di Sistema

Il progetto si articola su tre pilastri ingegneristici principali, integrati in modo sinergico:

[MECCANICA (Fusion 360)]
  * Testata Janus 2D porta-piezo inclinati a 30 gradi
  * Dimensionamento spessore per 1.5 MPa (150m di profondita max)
  * Resina di matching poliuretanica spessa mezza lunghezza d'onda (0.75 mm)
  * Chassis cilindrico a doppia tenuta O-ring radiale/assiale (NBR 70/90 Shore A)
  * Connettore subacqueo wet-mateable MCBH a 8 pin sul tappo posteriore
         |
         v (Integrazione Fisica)
[ELETTRONICA (KiCad 10 Shield THT/DIP)]
  * Integrati principali in comodi formati a foro passante (PDIP-8 / PDIP-16)
  * Alimentazione duale analogica +5V/-5V con pompa di carica LTC1144CN8
  * Pilotaggio diretto dei piezo a 1200 pF dal driver MAX4427CPA+ (corrente picco 1.5A)
  * Protezione d'ingresso T/R Switch e guadagno variabile analogico AD600JNZ (TVG via DAC)
  * Digitalizzazione ad alta velocita con scheda figlia ADC duale parallelo AD9226 (12-bit)
         |
         v (Acquisizione parallela via GPIO D & E con DMA sincrono)
[FIRMWARE (STM32H7 C/C++ su Nucleo)]
  * Acquisizione sincrona e parallela delle porte GPIOD/GPIOE tramite trigger hardware TIM2
  * Demodulazione digitale IQ a 1 MHz, filtraggio FIR (CMSIS-DSP) e autocorrelazione Rummler
  * Compensazione in tempo reale della velocita del suono tramite equazione Clay-Medwin
  * Generazione di telemetria seriale in formato Waterlinked JSON nativo per ArduPilot
  * Sistema di sicurezza interno con arresto TX in caso di allarme leak detector (PC5)
