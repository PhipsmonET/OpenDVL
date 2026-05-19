# Meccanica di OpenDVL (Autodesk Fusion 360)

Questo modulo raccoglie i disegni 2D, i modelli CAD 3D e le specifiche costruttive della struttura meccanica dell'OpenDVL. L'intero sistema è modellato in Autodesk Fusion 360 ed è ottimizzato per resistere all'ambiente sottomarino corrosivo e alle pressioni idrostatiche, alloggiando lo shield elettronico e i due trasduttori da 1 MHz.

---

## Plan di Design Meccanico

Il DVL è protetto da un contenitore stagno cilindrico di precisione. La configurazione si divide in tre sezioni principali:

Tappo Posteriore (Connettore Subacqueo wet-mateable MCBH)
     |
  [Doppio O-Ring]
     |
Tubo Pressione (Alloggiamento Shield Nucleo + Elettronica)
     |
  [Doppio O-Ring]
     |
Testata Janus 2D (2 Trasduttori piezo da 1 MHz inclinati a 30 gradi)

---

## Dettaglio dei Componenti e Scelte dei Materiali

### 1. Testata dei Trasduttori (2-Beam Janus Head)
- Geometria: Ospita 2 trasduttori piezoelettrici da 1 MHz orientati a 30 gradi rispetto all'asse centrale (verticale) del DVL.
  - Il Trasduttore 1 è inclinato a +30 gradi (in avanti) per misurare la componente anteriore.
  - Il Trasduttore 2 è inclinato a -30 gradi (all'indietro) per misurare la componente posteriore.
  - Questa configurazione a due fasci contrapposti permette di isolare la velocità orizzontale di avanzamento (Vx) e la velocità verticale di discesa/salita (Vz) cancellando le componenti di disturbo simmetriche.
- Materiale: POM-C (Delrin) nero o Alluminio 6061-T6 con anodizzazione dura a spessore (Type III). Il POM-C è preferito per la testata poiché garantisce isolamento galvanico totale, ottima stabilità dimensionale all'assorbimento di umidità e lavorazione meccanica di precisione su tornio o fresa CNC.
- Incapsulamento e Matching Layer a 1 MHz:
  - La frequenza elevata di 1 MHz corrisponde a una lunghezza d'onda in acqua di circa 1.5 mm.
  - Per ottimizzare il trasferimento di energia acustica dall'elemento piezoelettrico all'acqua senza generare riflessioni interne distruttive, lo spessore dello strato di resina poliuretanica acusticamente trasparente (Scotchcast 2131 o poliuretani per trasduttori) sul lato bagnato deve essere pari a un quarto di lunghezza d'onda (circa 0.37 mm) o a mezza lunghezza d'onda (circa 0.75 mm).
  - La modellazione 3D in Fusion 360 prevede una sede ad incasso per il piezo profonda esattamente lo spessore del disco piezoelettrico più lo spessore controllato dello strato di accoppiamento.

### 2. Tubo a Pressione (Pressure Hull)
- Dimensioni: Ottimizzato con un diametro interno sufficiente a ospitare una scheda di sviluppo STM32H7 Nucleo-144 (larghezza circa 70-80 mm) accoppiata al nostro shield. Il diametro interno minimo consigliato è di 85-90 mm, con uno spessore di parete di 5-6 mm in alluminio o 7-8 mm in POM-C per profondità fino a 300 metri.
- Materiali Consigliati:
  - POM-C (Delrin): Ideale per piccoli ROV e profondità moderate. Evita accoppiamenti galvanici corrosivi con le viti in acciaio inox e non richiede anodi sacrificali.
  - Alluminio 6061-T6: Anodizzato duro (mil-spec MIL-A-8625 Type III) per profondità operative elevate. È richiesto l'uso di pasta anti-grippaggio al rame/nichel sulle viti e un piccolo anodo sacrificale in zinco in contatto elettrico con lo chassis per prevenire la corrosione in acqua salata.

### 3. Sistema di Tenuta a O-Ring
- Standard: Dimensioni delle scanalature ed O-ring calcolate secondo le tabelle del Parker O-Ring Handbook.
- Configurazione a Doppia Tenuta:
  - O-Ring Radiale: Posizionato sul diametro di accoppiamento (spigot) del tappo. Funge da tenuta principale a compressione costante (circa 15-20% di schiacciamento della sezione).
  - O-Ring Assiale: Posizionato sulla faccia flangiata di battuta. Offre una barriera ridondante e beneficia dell'incremento di pressione esterna, che spinge la testata a sigillare maggiormente contro l'estremità del tubo.
- Durezza: Nitrile (NBR) 70 Shore A o 90 Shore A (consigliata per pressioni superiori a 15-20 bar per evitare l'estrusione nella luce di accoppiamento).

### 4. Connessioni e Interfaccia Esterna
- Passacavi / Connettori: Utilizzo di connettori subacquei wet-mateable standard (es. SubConn MCBH a 6 o 8 pin) per portare all'esterno l'alimentazione a +12V/+24V della batteria, il bus di telemetria seriale RS485 half-duplex (2 fili) e un pin opzionale di trigger esterno.

---

## Struttura della Cartella mechanics/

- drawings/: Disegni 2D (PDF/DXF) con tolleranze accoppiamento H7/g6 per le sedi O-ring e rugosità di finitura Ra <= 0.8 micrometri per prevenire infiltrazioni capillari di acqua lungo le lavorazioni del tornio.
- exports/:
  - OpenDVL_2Beam_Assembly.step: Assieme 3D completo.
  - Janus_2Beam_Head.step: La testata porta-piezo da 1 MHz inclinati a 30 gradi.
  - End_Cap.step: Il tappo posteriore per connettore SubConn.

---

## Workflow Fusion 360 Consigliato
1. Top-Down Design: Modella lo shield KiCad (esportato in STEP) all'interno dell'assieme Fusion 360 per verificare visivamente l'altezza dei componenti (il condensatore LTC1144, il chip AD600 e il modulo AD9226) rispetto alla parete interna del tubo a pressione e assicurare il corretto allineamento dei fori di fissaggio con la scheda Nucleo.
2. Tabella Parametri (Modify > Change Parameters):
   - Tube_ID (Diametro interno tubo, calibrato sulla larghezza della scheda Nucleo-144)
   - Tube_Wall (Spessore parete)
   - Janus_Angle (Impostato a 30 gradi per l'angolo del fascio)
   - Piezo_Diameter (Diametro dei trasduttori da 1 MHz)
   - Piezo_Thickness (Spessore dei dischi piezo)
