# Proposte Circuitale per lo Shield OpenDVL

Questo documento definisce le soluzioni circuitali ottimizzate in base alle specifiche dei tuoi trasduttori da 1 MHz e alla preferenza per un montaggio misto orientato a componenti a foro passante (THT / DIP).

---

## 1. Analisi delle Caratteristiche del Trasduttore 1 MHz

I trasduttori acustici selezionati presentano parametri che condizionano direttamente le scelte elettroniche e meccaniche:
- Frequenza: 1 MHz (Banda passante 0.2 MHz, Q = 5). Ottimo per impulsi brevi senza risonanze residue prolungate.
- Capacita statica (C0): 1200 pF +/- 20%. Rappresenta un carico capacitivo notevole a 1 MHz.
- Resistenza parallela (Rp): 130 Ohm. Indica l'impedenza reale alla risonanza, richiedendo un driver a bassa impedenza d'uscita.
- Limite di tensione (Impulso): 500 Vp-p max. Permette ampi margini di incremento di potenza futuri.
- Resistenza alla pressione: 1.5 MPa. Limita la profondita operativa massima a 150 metri (15 bar), definendo il vincolo meccanico per il tubo a pressione.

---

## 2. Scelta dei Package (Filosofia THT/DIP)

Per facilitare la prototipazione manuale e la saldatura su circuito stampato, i circuiti integrati principali sono selezionati nei seguenti formati a foro passante:
- Amplificatore AD600JNZ: Package PDIP-16 (passo 2.54 mm).
- Switched-Capacitor Inverter LTC1144CN8#PBF: Package PDIP-8.
- Gate Driver MAX4427CPA+: Package PDIP-8.
- I diodi Schottky, i preamplificatori (LNA), i regolatori LDO e i componenti passivi (resistenze e condensatori) utilizzeranno formati a foro passante standard (resistenze assiali da 0.25W, condensatori ceramici a disco o multistrato radiali). Il modulo ADC AD9226 puo essere montato come scheda figlia tramite strisce di connettori maschio/femmina (pin headers) passo 2.54 mm.

---

## 3. Stadio TX: Pilotaggio del Carico Capacitivo (1200 pF)

L'impedenza reattiva di un condensatore da 1200 pF a 1 MHz e pari a circa 132 Ohm. Per caricare e scaricare rapidamente questa capacita mantenendo fronti d'onda ripidi, e necessaria una corrente di picco significativa. 

Se operiamo con una tensione di alimentazione di 12V e puntiamo a un tempo di salita (t_rise) di 20 ns per minimizzare le perdite:
I_picco = C * (dV / dt) = 1200 pF * (12V / 20 ns) = 0.72 A.

Il MAX4427CPA+ e in grado di erogare picchi di corrente fino a 1.5 A, rendendolo perfetto per questo scopo. Si propongono due opzioni di pilotaggio:

### Opzione A: Pilotaggio Diretto dal MAX4427CPA (Consigliata per Prototipazione)
Data la bassa impedenza d'uscita del MAX4427 (circa 4 Ohm) e la portata massima di 5 metri del trasduttore, e possibile pilotare il piezo direttamente dalle uscite del driver, senza MOSFET di potenza esterni:
- Il pin 7 (OUTA) si collega al Trasduttore 1 tramite un condensatore di blocco in continua da 100 nF (poliestere o ceramico, 100V) in serie a una resistenza di limitazione da 10 Ohm (per smorzare le oscillazioni parassite con i 1200 pF del piezo).
- Il pin 5 (OUTB) si collega al Trasduttore 2 con la stessa configurazione.
- Questa soluzione riduce drasticamente il numero di componenti, l'area del PCB e la complessita del montaggio manuale.

### Opzione B: Semiponte Discreto con MOSFET TO-220
Se in futuro si desidera aumentare la tensione di trasmissione oltre i 15V (limite del MAX4427) per spingersi verso i 500 Vp-p:
- Il MAX4427CPA+ viene utilizzato come gate driver per pilotare MOSFET di potenza discreti in package TO-220 (es. IRF540 per canale N, IRF9540 per canale P).
- Il MAX4427 pilota i gate tramite resistenze da 10 Ohm. Il semiponte e alimentato da una linea a tensione piu elevata (es. 24V o superiore).

---

## 4. Alimentazione Duale con LTC1144CN8#PBF (THT)

L'inverter LTC1144CN8 converte la tensione di +5V in -5V stabili per alimentare l'AD600JNZ in package DIP.

### Schema di Collegamento THT
- Pin 2 e Pin 4: Collegati a un condensatore radiale ceramico multistrato da 10 uF (50V, Low-ESR) per il trasferimento di carica.
- Pin 5 (V_OUT): Connesso a un condensatore elettrolitico radiale da 47 uF (25V) per il livellamento dell'uscita.
- Per eliminare il ripple di commutazione prima che entri nella linea di alimentazione analogica negativa dell'AD600JNZ (AVEE, Pin 9), si inserisce un filtro LC passa-basso passivo composto da un'induttanza assiale a foro passante da 10 uH e un condensatore ceramico radiale da 10 uF.
- Pin 3 (SHDN): Collegato al pin PC4 della Nucleo tramite una resistenza assiale da 100 Ohm. Un resistore di pull-down assiale da 10k Ohm assicura che l'inverter sia attivo di default.

---

## 5. Catena RX Analogica (AFE) con AD600JNZ (PDIP-16)

### Protezione d'Ingresso (T/R Switch THT)
Il segnale di trasmissione (anche se a soli 5V o 12V) distruggerebbe l'ingresso dell'AD600JNZ se non protetto:
- Si utilizza una resistenza assiale da 220 Ohm (0.25W) in serie alla linea di ricezione.
- Subito dopo, due diodi Schottky rapidi in package a foro passante (es. 1N4148 o BAT85) connessi in antiparallelo verso la massa analogica (AGND). Durante il ping TX, i diodi limitano la tensione d'ingresso dell'LNA a +/-0.6V.

### Preamplificatore LNA THT
- Si utilizza un amplificatore operazionale a foro passante compatibile con alimentazione duale (es. LT1357 o AD829, ad altissima velocità e basso rumore) configurato con guadagno non invertente di +20 dB (resistenze assiali di retroazione da 909 Ohm e 100 Ohm verso massa).

### Variable Gain Amplifier (AD600JNZ)
L'AD600JNZ in formato PDIP-16 offre un cablaggio spazioso:
- Input: L'uscita dell'LNA e collegata al pin 1 (AINHI) del Canale 1 tramite un condensatore radiale da 10 nF. Il pin 2 (AINLO) e collegato direttamente al piano AGND. Il Canale 2 utilizza i pin 10 (AINHI) e 9 (AINLO) per il secondo trasduttore.
- TVG Control:
  - Pin 16 (V_G1-) e Pin 13 (V_G2-) sono collegati a AGND.
  - Pin 15 (V_G1+) e Pin 14 (V_G2+) ricevono la rampa di tensione analogica dai canali DAC della Nucleo (PA4 e PA5) attraverso filtri RC passa-basso (resistenza assiale da 1k Ohm e condensatore ceramico radiale da 100 nF).
- Outputs: Le uscite amplificate sui pin 8 (AOUT) e 11 (BOUT) sono inviate ai filtri attivi passa-banda a 1 MHz prima di entrare nel modulo AD9226.
