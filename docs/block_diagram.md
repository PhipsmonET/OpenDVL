# Schema a Blocchi della Circuiteria di OpenDVL

Questo documento contiene lo schema a blocchi funzionale completo dell'elettronica dell'OpenDVL, dettagliando le connessioni tra la scheda madre STM32H7 Nucleo, lo shield custom (HAT), l'ADC duale parallelo AD9226, l'analog front-end (AFE) e le connessioni esterne.

---

## Schema a Blocchi Funzionale

```text
========================================================================================================================
                                          SHIELD CUSTOM (HAT) + NUCLEO H7
========================================================================================================================

                                          +-----------------------------------------+
                                          |          CONNETTORE SUBACQUEO           |
                                          |            SubConn MCBH-8M              |
                                          +-------------------+---------------------+
                                                              |
                                      +-----------------------+-----------------------+
                                      | (Alimentazione 12-24V)| (Telemetria RS485)    | (Trigger/Leak)
                                      v                       v                       v
+------------------+         +-----------------+      +---------------+      +-------------------+
|                  |         |  CONVERTITORE   |      |  RICETRASMET. |      |  LINEE SYNC/LEAK  |
|  NUCLEO H7       |         |  STEP-DOWN DC   |      |  RS485        |      |  (Optoisolate e   |
|                  |         |  (V_BATT -> 5V) |      |  (MAX3485)    |      |  Protezioni TVS)  |
|                  +         +--------+--------+      +-------+-------+      +---------+---------+
|                  |                  |                       |                        |
|                  |                  v                       |                        |
|   +--------------+--------+  +---------------+              |                        |
|   | ALIMENTAZIONE DIGITALE|  | REGOLATORE    |              |                        |
|   | (+5V, +3.3V, GND)    |  | LDO +5V ANA   |              |                        |
|   +--------------+--------+  +-------+-------+              |                        |
|                  |                  |                       |                        |
|                  |                  +----------+            |                        |
|                  |                  |          |            |                        |
|                  |                  v          v            |                        |
|                  |           +-----------+  +--------+      |                        |
|                  |           |   AVDD    |  |LTC1144 |      |                        |
|                  |           |  (+5V)    |  |Inverter|      |                        |
|                  |           +-----------+  +----+---+      |                        |
|                  |                               |          |                        |
|                  |                               v          |                        |
|                  |                          +--------+      |                        |
|                  |                          | Filtro |      |                        |
|                  |                          |   LC   |      |                        |
|                  |                          +----+---+      |                        |
|                  |                               |          |                        |
|                  |                               v          |                        |
|                  |                          +--------+      |                        |
|                  |                          |  AVEE  |      |                        |
|                  |                          | (-5V)  |      |                        |
|                  |                          +--------+      |                        |
|                  |                                          |                        |
|                  |                                          |                        |
|   +--------------+------------------------------------------+------------------------+
|   | CONNETTORI ST MORPHO                                                             |
|   +--------------+------------------------------------------+------------------------+
|                  |                                          |                        |
|   (PC4 - SHDN)   +------------------------------------------+------------------------+ (LTC1144 SHDN)
|                  |                                          |
|   (PA9/PA10 UART)+------------------------------------------+------------------------+ (MAX3485 TX/RX)
|                  |                                          |
|   (PC5 - Leak)   +------------------------------------------+----+
|                  |                                               |
|                  |                                               v
|                  |                                    +--------------------+
|                  |                                    | LEAK DETECTOR      |
|                  |                                    | (Piste esposte)    |
|                  |                                    +--------------------+
|                  |
|                  |   ================ STADIO DI TRASMISSIONE (TX) ================
|                  |
|   (PE9 - TIM1)   +------------> +---------------+
|                  |              | GATE DRIVER   +-----> [ TRASDUTTORE 1 ]
|   (PE11 - TIM1)  +------------> | MAX4427CPA+   |       (1 MHz Piezo, C0=1200pF)
|                  |              | (DIP-8, 1.5A) +-----> [ TRASDUTTORE 2 ]
|                  |              +---------------+
|                  |
|                  |   ================= STADIO DI RICEZIONE (RX) ==================
|                  |
|                  |              +---------------+       +------------------+
|                  |              | T/R SWITCH    | <-----+ [ TRASDUTTORE 1 ]
|                  |              | (Protezione)  |       +------------------+
|                  |              +-------+-------+
|                  |                      |
|                  |                      v
|                  |              +---------------+
|                  |              | PREAMP (LNA)  |
|                  |              | (OPA835, THT) |
|                  |              +-------+-------+
|                  |                      |
|                  |                      v
|                  |              +---------------+
|   (PA4 - DAC1)   +------------> | VGA DUAL CH   |
|   (PA5 - DAC2)   +------------> | AD600JNZ      | (Rampa TVG di controllo guadagno)
|                  |              | (PDIP-16)     |
|                  |              +-------+-------+
|                  |                      |
|                  |                      v
|                  |              +---------------+
|                  |              | FILTRO ATTIVO |
|                  |              | PASSA-BANDA   | (1 MHz, Q=10 MFB Filter)
|                  |              | (OPA835, THT) |
|                  |              +-------+-------+
|                  |                      |
|                  |                      v
|                  |              +---------------+
|                  |              | TRASLATORE DI |
|                  |              | LIVELLO (VCOM)| (Centra il segnale a +/-1V su VCOM)
|                  |              +-------+-------+
|                  |                      |
|                  |                      v
|                  |              +---------------+
|   (PA8 - CLK)    +------------> | DUAL AD9226   | (Clock comune di campionamento)
|                  |              | PARALLEL ADC  |
|   (PD0-PD11)     |<-------------+ (12-bit Ch A) | (Acquisizione sincrona via DMA)
|   (PE0-PE11)     |<-------------+ (12-bit Ch B) | (Acquisizione sincrona via DMA)
|                  |              +---------------+
|                  |
|                  |   ================ COMPENSAZIONE ACUSTICA =====================
|                  |
|   (PB6/PB7 I2C)  +<-----------------------------------> +------------------+
|                  |                                      | SENSORE TEMP     |
|                  |                                      | (Sonda TSYS01)   |
|                  |                                      +------------------+
========================================================================================================================
