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
