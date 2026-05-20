# Integrazione di OpenDVL con ArduPilot

Questo documento descrive come interfacciare l'OpenDVL con l'ecosistema ArduPilot (in particolare con il firmware ArduSub per ROV e AUV), configurando il protocollo seriale, i collegamenti fisici e i parametri di navigazione della Pixhawk per ottenere un posizionamento stimato stabile in ambienti privi di segnale GPS (GPS-denied navigation).

---

## 1. Protocolli Supportati da ArduPilot (AP_DVL)

La libreria di navigazione di ArduPilot (`AP_DVL`) supporta nativamente due protocolli principali di comunicazione per i DVL:

1. **Protocollo Waterlinked A50 (JSON seriale)**:
   - Utilizza stringhe in formato JSON trasmesse via seriale UART (tipicamente a 115200 baud).
   - E estremamente semplice da generare lato firmware STM32H7 e facile da decodificare.
   - Poiche il nostro DVL lavora in configurazione Janus 2D, impostiamo la componente laterale (Vy) a zero, trasmettendo la velocita di avanzamento (Vx) e quella verticale (Vz).

2. **Protocollo Teledyne Marine (PD0 Binary)**:
   - Un formato binario compatto e denso, ampiamente utilizzato nel settore industriale subacqueo.

### Scelta Consigliata: Protocollo Waterlinked (JSON)
Per la nostra implementazione, la scelta piu robusta ed immediata e il **Protocollo Waterlinked JSON**. Questo ti evita di dover scrivere o modificare il codice sorgente di ArduPilot, poiche la Pixhawk riconosce nativamente questo formato e lo immette direttamente nel filtro Kalman Esteso (EKF3) per la stima della posizione.

---

## 2. Connessione Fisica: OpenDVL to Pixhawk (Flight Controller)

Il DVL comunica esternamente in **RS485 Half-Duplex** per garantire l'immunità al rumore indotto dai motori lungo il cablaggio interno del ROV. La Pixhawk (o qualsiasi flight controller equivalente) lavora invece con segnali seriali standard a livello TTL (3.3V).

### Schema di Collegamento
Per interfacciare le linee differenziali dell'OpenDVL con una delle porte seriali (es. TELEM1 o TELEM2) della Pixhawk, e necessario interporre un piccolo convertitore RS485-to-TTL (basato ad esempio sul chip MAX3485) sul lato del veicolo:

```text
+---------------------+                      +---------------------+
|  OpenDVL (Shield)   |                      | VEICOLO (ROV / AUV) |
|                     |                      |                     |
|  [ Nucleo H7 ]      |                      |  [ Convertitore ]   |
|   UART1 (PA9/PA10)  |                      |  [ RS485 to TTL ]   |
|         |           |                      |         |           |
|         v           |                      |         |           |
|  [ Transceiver ]    |   Cavo RS485 Stagno  |         v           |    Porta TELEM2
|  [ MAX3485 ]        |                      |  [ MAX3485 ]        |    della Pixhawk
|   RS485_A / RS485_B +======================+   RS485_A / RS485_B |    (Segnali TTL 3.3V)
+---------------------+                      |   TX / RX / VCC/GND +---> [ Pixhawk RX ] (Pin 3)
                                             +---------------------+---> [ Pixhawk TX ] (Pin 2)
                                                                     +---> [ Pixhawk GND ](Pin 6)
```

---

## 3. Struttura del Messaggio JSON Inviato dalla Nucleo

Il firmware STM32H7 e configurato per trasmettere periodicamente (es. a 5 Hz o 10 Hz) una stringa JSON sulla seriale USART1 formattata in questo modo:

```json
{"time":123.45,"vx":0.12,"vy":0.00,"vz":-0.05,"fom":0.02,"covariance":[[0.0001,0,0],[0,0.0001,0],[0,0,0.0001]],"altitude":1.24,"valid":true}
```

### Dettaglio dei Campi:
- `"time"`: Tempo di sistema in secondi dall'avvio della Nucleo.
- `"vx"`: Velocita longitudinale misurata (in m/s).
- `"vy"`: Velocita trasversale (impostata fissa a 0.00 nel nostro sistema 2D).
- `"vz"`: Velocita verticale misurata (in m/s).
- `"fom"`: Figure of Merit (indice di precisione della misura, es. deviazione standard in m/s). Valori tipici < 0.05.
- `"covariance"`: Matrice di covarianza associata all'errore di misura delle tre componenti di velocita.
- `"altitude"`: Distanza stimata dal fondale (in metri), derivata dal tempo di volo (ToF) dell'eco del ping.
- `"valid"`: Flag booleano (`true` se l'autocorrelazione ha superato la soglia di validita del segnale, `false` in caso di perdita del fondo o segnale debole).

---

## 4. Configurazione dei Parametri in ArduPilot

Per abilitare il DVL come sorgente di navigazione principale in ArduPilot, e necessario impostare i seguenti parametri tramite un software di stazione di terra (es. **QGroundControl** o **Mission Planner**):

### A. Abilitazione della Porta Seriale (es. TELEM2)
Assumendo che il convertitore RS485-to-TTL sia collegato alla porta seriale `SERIAL2` della Pixhawk:
- `SERIAL2_PROTOCOL` = **43** (DVL)
- `SERIAL2_BAUD` = **115** (115200 baud)

### B. Configurazione del Driver DVL
- `DVL_ENABLE` = **1** (Abilita il sottosistema DVL)
- `DVL_TYPE` = **1** (Imposta il tipo di protocollo su Waterlinked JSON)

### C. Configurazione del Filtro di Kalman Esteso (EKF3)
Per far si che il filtro di navigazione EKF3 utilizzi i dati di velocita del DVL invece del GPS (che sott'acqua e assente) per calcolare la posizione del veicolo:
- `EK3_SRC1_VELXY` = **5** (Usa DVL per la velocita orizzontale)
- `EK3_SRC1_VELZ` = **5** (Usa DVL per la velocita verticale)
- `EK3_SRC1_POSXY` = **0** (Disabilita il posizionamento assoluto orizzontale, poiche il DVL fornisce velocita relativa al fondo)
- `EK3_SRC1_POSZ` = **1** (Usa il sensore di pressione barometrico/profondita per la quota assoluta)

---

## 5. Gestione del Ping Sincronizzato (Trigger)

Nel software ArduSub, e comune avere piu strumenti acustici attivi contemporaneamente (es. altimetro acustico, sonar a scansione, USBL).
- Se questi dispositivi operano vicino alla frequenza di 1 MHz, possono generare gravi interferenze acustiche.
- Il pin di **Trigger Esterno (TRIG_EXT)** presente sul connettore dell'OpenDVL puo essere collegato a una delle uscite ausiliarie (AUX) della Pixhawk.
- ArduPilot puo essere configurato per comandare il pin di trigger in modo da coordinare temporalmente i ping dei vari sonar, evitando che l'OpenDVL trasmetta nello stesso istante in cui un altro sonar e in ascolto.
