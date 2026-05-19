# 🔩 Meccanica di OpenDVL (Autodesk Fusion 360)

Questo modulo raccoglie i disegni 2D, i modelli CAD 3D e le specifiche costruttive della struttura meccanica dell'OpenDVL. L'intero sistema è modellato in **Autodesk Fusion 360** ed è ottimizzato per resistere all'ambiente sottomarino corrosivo e alle alte pressioni idrostatiche.

---

## 📐 Concetto di Design Meccanico

Il DVL è ospitato all'interno di un contenitore stagno cilindrico di precisione. La configurazione si divide in tre parti principali:

```
                  ┌──────────────────────────────────────────────┐
                  │                 Tappo Posteriore             │
                  │  (Connettore Subacqueo wet-mateable, Anodo)  │
                  └──────────────────────┬───────────────────────┘
                                         │
                                 [Doppio O-Ring]
                                         │
                  ┌──────────────────────▼───────────────────────┐
                  │               Tubo Pressione                 │
                  │   (Alloggiamento Elettronica, Alluminio/POM) │
                  └──────────────────────┬───────────────────────┘
                                         │
                                 [Doppio O-Ring]
                                         │
                  ┌──────────────────────▼───────────────────────┐
                  │               Testata Janus                  │
                  │ (4 Trasduttori piezo inclinati a 30° + Temp) │
                  └──────────────────────────────────────────────┘
```

---

## 🔩 Dettaglio dei Componenti e Scelte dei Materiali

### 1. Testata dei Trasduttori (Janus Head)
- **Geometria**: Ospita 4 trasduttori piezoelettrici orientati a **$30^{\circ}$ rispetto all'asse centrale** (verticale) del DVL e sfalsati di $90^{\circ}$ sul piano azimutale. Questa configurazione consente di calcolare le velocità $V_x$, $V_y$ e $V_z$ sfruttando la combinazione lineare delle letture dei 4 fasci acustici (con ridondanza per tollerare l'eventuale perdita di segnale su un fascio).
- **Materiale**: **POM-C (Derlin)** nero o **Alluminio 6061-T6** con anodizzazione dura a spessore (Type III). Il POM-C è preferito per la testata in quanto offre un eccellente isolamento galvanico, ottima lavorabilità al tornio/fresa CNC e non è soggetto a corrosione.
- **Incapsulamento**: I dischi piezoelettrici sono alloggiati in apposite cavità lavorate, sigillati sul retro con resina epossidica impermeabile e protetti sul lato bagnato da una colata di **resina poliuretanica acusticamente trasparente** (es. *Scotchcast 2131* o poliuretani specifici per trasduttori), che garantisce l'accoppiamento acustico perfetto con l'acqua senza attenuare il segnale.

### 2. Tubo a Pressione (Pressure Hull)
- **Dimensioni Tipiche**: Diametro esterno $90\text{ mm} - 110\text{ mm}$, spessore della parete di $5\text{ mm} - 8\text{ mm}$ (a seconda del materiale e della profondità operativa mirata).
- **Materiali Consigliati**:
  - **Alluminio 6061-T6 o 7075-T6** con anodizzazione dura (mil-spec MIL-A-8625 Type III) per profondità fino a $1000\text{ m}$. Richiede un anodo sacrificale in zinco per prevenire la corrosione galvanica se accoppiato con viti in acciaio inossidabile 316.
  - **POM-C (Delrin)** per profondità moderate (fino a $300\text{ m}$). Non ha problemi di corrosione galvanica e riduce il peso complessivo.
  - **Fibra di Carbonio** o **Titanio Gr. 5** per applicazioni offshore/abissali estreme (oltre $3000\text{ m}$).

### 3. Sistema di Tenuta a O-Ring
- **Standard**: Dimensioni O-Ring e scanalature calcolate secondo lo standard **Parker O-Ring Handbook**.
- **Configurazione**: Doppia tenuta sia sulla testata anteriore sia sul tappo posteriore.
  - Primo O-Ring: **Tenuta Radiale** (funge da tenuta principale a deformazione controllata).
  - Secondo O-Ring: **Tenuta Assiale** (posizionato sulla battuta piatta, garantisce ulteriore sicurezza quando la pressione idrostatica spinge il tappo contro il tubo).
- **Mescola**: Nitrile (NBR) con durezza **70 Shore A** (per applicazioni generali) o **90 Shore A** (per pressioni elevate per evitare l'estrusione dell'O-ring nella luce di accoppiamento).

### 4. Connessioni e Interfaccia Esterna
- **Passacavi / Connettori**: Utilizzo di connettori subacquei wet-mateable standard (es. **SubConn MCBH** a 4, 6 o 8 pin) o penetratori stagni ad alta pressione con guarnizione a compressione (es. *Blue Robotics Penetrators*).
- **Segnali**: Alimentazione ($12\text{V} - 24\text{V}$), bus seriale (RS232/RS485 o Ethernet a 4 fili) e linea di sincronizzazione (trigger esterno).

---

## 📂 Struttura della Cartella `mechanics/`

All'interno di questo modulo troverai:
- `drawings/`: Disegni costruttivi 2D (PDF e DXF) completi di tolleranze geometriche H7/g6 per le sedi O-ring e rugosità superficiali raccomandate ($Ra \le 0.8\ \mu\text{m}$ nelle sedi di tenuta dinamica/radiale).
- `exports/`:
  - `OpenDVL_Assembly.step`: Il modello CAD completo assemblato in formato STEP neutro per l'importazione in altri CAD.
  - `Janus_Head.step` e `End_Cap.step`: I singoli componenti meccanici principali esportati per la lavorazione CNC.
  - `Transducer_Mold.stl`: Stampo modellato per la colata del poliuretano di protezione sui piezo.

---

## 🔧 Workflow Fusion 360 Consigliato
1. **Top-Down Design**: Si consiglia di modellare l'intero assieme all'interno di un unico file di Fusion 360 usando i *Componenti* anziché modellare parti separate e importarle. Questo mantiene i riferimenti parametrici sempre attivi (ad esempio, modificando il diametro del tubo si adegueranno automaticamente i tappi e le sedi degli O-ring).
2. **Tabella Parametri**: Utilizzare la funzione `Modify > Change Parameters` in Fusion 360 per definire le variabili chiave del progetto:
   - `Tube_OD` (Diametro esterno tubo)
   - `Tube_Wall` (Spessore parete tubo)
   - `Oring_Groove_Depth` (Profondità scanalatura O-ring)
   - `Oring_Groove_Width` (Larghezza scanalatura O-ring)
   - `Janus_Angle` (inclinazione dei trasduttori, preimpostata a 30 gradi)
3. **Controllo Gioco di Accoppiamento**: Nelle accoppiate radiali tra tappo e tubo, mantenere una tolleranza diametrale di gioco compresa tra $0.05\text{ mm}$ e $0.1\text{ mm}$ per facilitare l'inserimento manuale e consentire all'O-ring di lavorare correttamente senza estrudersi.
