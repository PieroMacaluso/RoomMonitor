<h1 align="center">
  <a href="https://github.com/pieromacaluso/RoomMonitor" title="RoomMonitor Documentation">
    <img alt="RoomMonitor" src="RoomMonitor/resources/logo.svg" width="200px" height="200px" />
  </a>
  <br/>
</h1>

<p align="center">
  Counting people using Wi-Fi devices.
</p>

<p align="center">
 <img alt="Languages" src="https://img.shields.io/badge/Languages-C%20|%20C%2B%2B-orange"/>
 <img alt="Framework" src="https://img.shields.io/badge/Framework-Qt-green"/>
 <img alt="Status" src="https://img.shields.io/badge/Status-WIP-yellow"/>
</p>

## Indice

- [Indice](#indice)
- [Team di sviluppo](#team-di-sviluppo)
- [Codice sorgente progetti](#codice-sorgente-progetti)
- [Introduction](#introduction)
  - [Scansione Attiva](#scansione-attiva)
  - [Dispositivo ESP32](#dispositivo-esp32)
  - [Obiettivo del progetto](#obiettivo-del-progetto)
- [ESP32 Firmware](#esp32-firmware)
  - [Scopo](#scopo)
  - [Configurazione](#configurazione)
  - [Funzionamento](#funzionamento)
  - [Spiegazione tecnica](#spiegazione-tecnica)
    - [Captive Portal](#captive-portal)
    - [Sniffing](#sniffing)
    - [Validazione e Inoltro](#validazione-e-inoltro)
  - [Indicazione stato tramite led](#indicazione-stato-tramite-led)
- [RoomMonitor Server](#roommonitor-server)
  - [Impostazioni](#impostazioni)
    - [Stanza](#stanza)
    - [Monitoraggio](#monitoraggio)
    - [Database](#database)
    - [MAC Nascosto](#mac-nascosto)
  - [Monitoraggio](#monitoraggio-1)
    - [Aggregazione](#aggregazione)
  - [Trilaterazione](#trilaterazione)
  - [Analisi](#analisi)
- [Sviluppi Futuri](#sviluppi-futuri)

Questo progetto è stato sviluppato partendo dalla [proposta di progetto](stuff/specs.pdf) fornita dal prof. Giovanni Malnati durante il corso "Programmazione di Sistema" nell' a.a. 2017/2018.

## Team di sviluppo

- <img alt="avatar" src="https://github.com/pieromacaluso.png" width="20px" height="20px"> **Piero Macaluso** - [pieromacaluso](https://github.com/pieromacaluso)
- <img alt="avatar" src="https://github.com/lorenzomanicone.png" width="20px" height="20px"> **Lorenzo Manicone** - [lorenzomanicone](https://github.com/lorenzomanicone)
- <img alt="avatar" src="https://github.com/angeloturco.png" width="20px" height="20px"> **Angelo Turco** - [angeloturco](https://github.com/angeloturco)

## Codice sorgente progetti

- [**RoomMonitor ESP32 Firmware**](ESP32firmwareCLion)
- [**RoomMonitor Server App**](RoomMonitor)

## Introduction

### Scansione Attiva

Ogni dispositivo WiFi, in particolare gli smartphone, possiedono la capacità di riconnettersi rapidamente ad una rete WiFi precedentemente associata non appena si vengono a trovare all'interno dell'area di copertura. Questa funzionalità è possibile grazie allo sfruttamento della *scansione attiva* dei nostri dispositivi: questi inviano, ad intervalli regolari, pacchetti broadcast di tipo **PROBE REQUEST** indicando in maniera facoltativa l'SSID per cui si richiede il collegamento.

### Dispositivo ESP32

Il dispositivo ESP32 utilizzato in questo progetto possiede un'implementazione dello stack WiFi, il quale permette allo stesso di registrare una callback da invocare ogni volta che viene ricevuto un pacchetto di tipo **CONTROL**, rilevando sia **BEACON**, sia **PROBE REQUEST**.

All'interno di questo pacchetto è possibile trovare la potenza del segnale ricevuto *RSSI (Received Signal Strenght Indicator)*. In condizioni ideali questo valore decresce esponenzialmente al crescere della distanza ed viene espresso in *dB*. I valori tipici variano tra -35/-40dB (molto vicino), fino a -95/-105dB (molto lontano).

Nella realtà sono presenti numerosi fattori che possono condizionare tale valore come ad esempio la presenza di riflessioni multiple dovute al terreno, ai muri e agli altri ostacoli, alla propagazione o alle interferenze con altri segnali presenti nell'ambiente.

In ogni caso rimane uno strumento utile per poter misurare la distanza del dispositivo.

### Obiettivo del progetto

L'obiettivo principale del software implementato è quello di fornire un sistema capace di monitorare numero e posizione di studenti all'interno di un'aula. Questo è possibile sfruttando i pacchetti PROBE REQUEST utilizzati dai dispositivi che normalmente utilizziamo: questi vengono catturati da una serie di schedine ESP32 (2 o più) per poi essere trasmessi ad un server che si occupa di filtrare e aggregare i pacchetti ricevuti da tutte le schedine in azione per ottenere l'informazione sulla posizione partendo dai valori dell'RSSI.
Da questi valori è possibile passare ad un valore della distanza del singolo dispositivo (identificato dal suo MAC) da ogni schedina, utile per procedere con una triangolazione della posizione.

## ESP32 Firmware

### Scopo

Lo scopo della board ESP32 è quello di sniffare i vari pacchetti **PROBE REQUEST** generati dai dispositivi vicini ed inoltrarli al server il quale si occuperà di gestirli.

### Configurazione

In fase di montaggio del sistema, occorre un addetto per inizializzare i corretti valori necessari per il funzionamento.
Per garantire una facile configurazione è stato implementato un *captive portal* dove poter introdurre i valori necessari senza dover modificare il firmware.
Per collegarsi a tale portale occorre connettersi alla rete Wi-Fi della board che si vuole configurare (default SSID: `esp32-ap`, Password: `progettopds`), dopodichè, tramite un browser (nota Chrome può avere problemi), collegarsi all'indirizzo `https://192.168.1.1`. (Nota, attenzione a non usare http ma **https**).
A questo punto verrà mostrata la pagina di configurazione della scheda dove poter inizializzare tutti i parametri necessari.
<div align="center">

![Captive Portal](stuff/img/captive.jpg)

</div>

Come si vede è possibile cambiare le impostazioni della rete WiFi della scheda, assegnarle un ID (importante per configurazione server), e il canale su cui si vuole effettuare la scansione. Nella parte sottostante è possibile impostare tutti i parametri per la comunicazione con il server.
L’invio di questi parametri è reso sicuro tramite il protocollo **HTTPS**, garantendo confidenzialità, integrità ed autenticazione, invece per l’inoltro dei pacchetti sniffati verso il server attraverso canale TCP sono garantite le proprietà di autenticazione e integrità tramite l'utilizzo di HMAC-SHA256 con chiave a 256 bit per ogni pacchetto inoltrato.

### Funzionamento

Una volta configurata, la board si collegherà alla rete wifi del server. È importante che tale rete abbia una connessione ad internet in quanto la scheda userà tale connessione per ricevere l’orario corretto tramite un server SNTP.
Solo dopo aver acquisito l’orario, verrà avviata la procedura di sniffing e di inoltro.
Durante la fase di sniffing, verranno catturati tutti i vari PROBE REQUEST trasmessi sul canale indicato in fase di configurazione. Da tali pacchetti verranno estratte le informazioni necessarie e solo quando la fase di sniffing sarà terminata, tutti i dati acquisiti saranno inviati al server per poi riprendere con la prima fase.
Grazie all’utilizzo di un timer ogni minuto si provvede ad alternare la fase di sniffing e di inoltro. Per garantire il corretto sincronismo tra le varie schede e il server, lo stesso timer indica anche quando aggiornare l’orario eseguendo un’ulteriore richiesta al server SNTP.

### Spiegazione tecnica

#### Captive Portal

Per realizzare il meccanismo del captive portal è stato implementato un web server basato su HTTPS. Tale server risulta raggiungibile tramite l’indirizzo ip `192.168.1.1` sulla porta `443`. Sono stati definiti file HTML, CSS e Javascript che permettono l’inserimento dei vari parametri di configurazione.
È stato necessario anche l’uso di un certificato e di un file contenente la chiave privata per HTTPS.

L’utilizzo di apposite partizione di memorie ha permesso di memorizzare i valori impostati e i vari file utilizzati per implementare il server, in particolare si è deciso di usare una partizione **NVM**, tipo chiave-valore, per memorizzare i vari valori di inizializzazione. Tale partizione è basata su memoria non volatile che permette il salvataggio dei parametri anche in assenza di tensione.
Per gestire la partizione dedicata al captive portal è stato usato un apposito file system chiamato **SPIFFS** (Serial Peripheral Interface Flash File System).
Prima di memorizzare i valori inseriti tramite il browser, essi vengono validati e solo in caso di correttezza saranno memorizzati e verrà indicato all’utente che la fase di configurazione è terminata correttamente.
 
#### Sniffing

In tale fase la board intercetta i pacchetti dei vari dispositivi. Per far ciò si utilizza la modalità promiscua, attraverso la quale, è possibile ricevere ed elaborare pacchetti non destinati al proprio dispositivo.
Ogni qualvolta un pacchetto sarà intercettato dalla board esso sarà gestito da un apposito handler il quale si occuperà per prima cosa di filtrare il pacchetto ricevuto, accettandolo solo in caso di pacchetto con tipo 'Management'(`type= 0x00`) e sottotipo 'Probe Request' (`Subtype= 0x0b0100`), in seguito provvede ad aggiungerlo alla lista dei pacchetti ancora non inviati al server.
Per comodità di debug e di utilizzo è stato deciso di stampare a video i dati rilevanti del pacchetto accettato.

#### Validazione e Inoltro

Dopo il periodo di sniffing occorre inviare i dati acquisiti al server tramite un client Tcp.
Per prima cosa occorre impostare correttamente la scheda di rete per disabilitare la modalità promiscua e per configurare correttamente il socket per la connessione con il server solo dopo sarà possibile inviare la lista dei pacchetti acquisiti.
Ogni pacchetto sarà inviato come una stringa formata da due parti separati dal carattere `|` , una prima parte contenente i dati reali del pacchetto e una seconda parte contenente il calcolo del HMAC-SHA256 eseguito sulla prima porzione di stringa. 
La prima parte della stringa conterrà i seguenti dati: id_board, fcs_packet, rssi_packet, mac_src_packet, timestamp, SSID, mac_board.
Dovendo inviare più pacchetti si è deciso di concatenare ogni stringa, corrispondente all'iesimo pacchetto, con quella ottenuta dal pacchetto precedente separata dal carattere `;`.
Una volta inviati tutti i dati acquisiti si provvede alla liberazione della varie strutture dati utilizzate per evitare memory leak. 
Per comodità di debug e di utilizzo è stato deciso di far lampeggiare il led a bordo della scheda per indicare il corretto funzionamento. In caso di errore durante l’invio dei pacchetti tale led rimarrà acceso fin quando non sarà nuovamente possibile collegarsi col server.

### Indicazione stato tramite led
Tramite un led installato sulla scheda sarà possibile capire lo stato del programma. 
Durante la fase di acquisizione dell’orario il led lampeggia alternando lo stato acceso e quello spento ogni 2 secondi circa, solo quando l’orario verrà impostato correttamente il led si spegnerà e inizierà la fase di sniffing.
Alla fine di tale fase, si provvederà ad inoltrare i dati al server, per indicare il corretto invio il led eseguirà un solo lampeggio, in caso di errore invece rimarrà acceso.


## RoomMonitor Server

Questo secondo componente del progetto rappresenta l'applicazione desktop per poter ricevere, manipolare e fare lo storage dei dati ricevuti dalle analisi delle diverse schede ESP32. Questa svolge funzioni di monitoraggio in tempo reale, ma anche funzioni di analisi dei dati passati.

### Impostazioni

#### Stanza

Le impostazioni della stanza permettono di inserire dati sulle caratteristiche della stanza dove si andrà ad effettuare il monitoraggio.

- **Larghezza e Lunghezza Stanza**: questi valori sono utili per poter disegnare a video il perimetro della stanza e per poter scartare i dati che non si trovano all'interno della stessa.
- **Schedine**: è possibile andare a inserire le schedine che vengono posizionate all'interno della stanza indicando ID univoco della schedina, posizione e potenza a un metro (dBm).

#### Monitoraggio

- **Costante Propagazione Segnale**: parametro utilizzato dall'algoritmo per passare da RSSI a distanza in metri. Questo parametro è settato di default a 2 o 3 per spazi quali aule o stanze.
- **Minimo minuti conteggio presenza**: la granularità con cui vengono effettuate le stime è di 5 minuti. Questo parametro indica quanti minuti sul totale di 5 sono necessari affinchè il software conteggi la persona come presente in quei 5 minuti.
- **Limite Fallimenti Aggregazione**: questo parametro indica qual'è il massimo numero di aggregazioni che il software farà senza ricevere nemmeno un singolo pacchetto da almeno una schedina
- **Porta Server TCP**: serve per specificare in quale porta il server TCP riceverà i dati dalle varie schedine.

#### Database

In questa finestra è possibile andare a specificare tutti i dati riguardanti il database in cui andare a salvare i dati aggregati ricevuti da tutte le schedine. Da questa facciata è possibile andare ad operare sul database inizializzando una nuova tabella per una nuova cattura o andando a ripristinare la configurazione delle schedine per una determinata stanza.

#### MAC Nascosto

In questa schermata è possibile andare a impostare le tolleranze e i pesi per ogni parametro da tenere in considerazione nella stima della somiglianza del MAC nascosto. E' possibile andare a specificare la tolleranza per la posizione, il tempo e l'SSID. I pesi rappresentano quanto sarà importante quel parametro nel calcolo della stima. Il calcolo della stima viene fatto utilizzando la seguente equazione dove `p` rappresenta un numero tra 0 e 1 dove 1 indica che la differenza di tempo/spazio tra due pacchetti è pari a 0 oppure che l'SSID è identico. Zero indica una differenza maggiore o uguale alla tolleranza o SSID diverso.

<div align="center">
<img src="https://latex.codecogs.com/gif.latex?\text{percentage&space;(\%)}&space;=&space;\frac{\sum_{i&space;\in&space;\{\text{time},&space;\text{space},&space;\text{ssid}\}}&space;p_i&space;\cdot&space;w_i}{\sum_{i&space;\in&space;\{\text{time},&space;\text{space},&space;\text{ssid}\}}&space;w_i}&space;\cdot&space;100" title="\text{percentage (\%)} = \frac{\sum_{i \in \{\text{time}, \text{space}, \text{ssid}\}} p_i \cdot w_i}{\sum_{i \in \{\text{time}, \text{space}, \text{ssid}\}} w_i} \cdot 100" />
</div>

Le percentuali così calcolate per ogni pacchetto vengono poi mediate sul numero di pacchetti riferiti alla stessa coppia di MAC per poter dare una stima di somiglianza tra i due indirizzi MAC.

### Monitoraggio

La parte principale del software è costituita dalla fase di monitoraggio dove il server si mette in ascolto delle connessione TCP provenienti dalle schedine configurate da cui riceve i dati per eseguire le stime.

#### Aggregazione
A livello di tempistiche, abbiamo deciso di implementare il sistema in modo che sia in grado di funzionare in maniera asincrona, lasciando uno strato di disaccoppiamento tra schedine e server di aggregazione.
Le schedine sono libere di mandare i pacchetti a cadenza regolare di un minuto al server mettendosi in coda nel canale TCP. I dati così ricevuti vengono accumulati in una coda di pacchetti unica di tipo second-chance. Ogni minuto il server provvederà a eseguire l'aggregazione su ogni singolo pacchetto: questo (identificato dal CRC a 32bit trasmesso dalla schedina) verrà salvato sul database ed eliminato dalla coda solo se sarà presente un numero di pacchetti pari al numero di schedine utilizzate.
In caso contrario ad ogni singolo pacchetto verrà incrementato il contatore di chance. Alla fine di ogni processo di aggregazione i contatori di ogni pacchetto vengono controllati per verificare se il pacchetto deve essere eliminato o mantenuto.

Questo approccio ha permesso di poter gestire i pacchetti ricevuti da tutte le schedine senza introdurre forzature da meccanismi sincroni.

### Trilaterazione

Quando per ogni pacchetto (identificato dal CRC 32 bit) sono disponibili dati provenienti da tutte le schedine configurate nella cattura, allora è il momento di andare a calcolare la posizione stimata del pacchetto e salvarlo nel database.

Per il calcolo della posizione nello spazio abbiamo deciso di utilizzare la trilaterazione. Questa necessita, in primis, di andare a convertire i valori di RSSI in metri. Questa procedura è stata possibile grazie alla seguente formula:

<div align="center">
<img src="https://latex.codecogs.com/gif.latex?\LARGE&space;\text{d(m)}&space;=&space;10^{\frac{A-\text{RSSI}}{10&space;\cdot&space;n}}" title="\LARGE \text{d(m)} = 10^{\frac{A-\text{RSSI}}{10 \cdot n}}" />
</div>

`A` rappresenta la potenza del segnale della schedina quando il dispositivo si trova ad un metro dalla stessa, mentre `n` è costante di propagazione del segnale che in spazi aperti o aule molto grandi è solitamente pari a 2 o 3 (nelle nostre analisi abbiamo utilizzato il 3).

Una volta ottenute le misure delle distanze abbiamo di fatto ottenuto tanti cerchi quante sono le schedine: la loro posizione rappresenta il centro del cerchio, mentre la distanza il raggio.
Da questi dati è possibile andare a stimare la posizione del dispositivo.

Inizialmente abbiamo pensato di ingrandire o diminuire in egual misura tutti i cerchi disponibili (aumentando o diminuendo l'RSSI) in modo da avere un'area comune per tutti in cui andare a stimare la posizione del dispositivo attraverso i punti di intersezione. Questo approccio si è rivelato poco funzionale e instabile: in alcuni casi si andava a finire in situazioni dove non si riuscivano ad aumentare o diminuire i cerchi in maniera coerente e l'algoritmo (progettato per continuare a fare tentativi finiti) concludeva l'analisi scartando il pacchetto.

Dopo aver effettuato una ricerca sui vari metodi per stimare la posizione abbiamo deciso di procedere con un metodo simile al precedente, ma che riesce ad essere più stabile con dovute semplificazioni.
Per poter stimare la posizione si prendono combinazioni di cerchi e si calcolano la loro intersezione aumentando il raggio per far in modo che ci sia almeno un punto di intersezione: in caso di due punti di intersezione si va a prendere quello più vicino ai centri degli altri cerchi. Una volta accumulati questi punti si procede al calcolo della media in maniera pesata, basandosi sui raggi delle circonferenze utilizzati per il calcolo di quella specifica misurazione. 

<div align="center">
<img src="https://latex.codecogs.com/gif.latex?\inline&space;\dpi{300}&space;\small&space;\text{num}_{x}&space;=&space;\sum_{i=0}^{n}&space;\frac{x_i}{\sum_{j=0}^{j=1}r_j}&space;\;\;\;&space;\text{num}_{y}&space;=&space;\sum_{i=0}^{n}&space;\frac{y_i}{\sum_{j=0}^{j=1}r_j}&space;\;\;\;&space;\text{den}&space;=&space;\sum_{i=0}^{n}&space;\frac{1}{\sum_{j=0}^{j=1}r_j}" title="\small \text{num}_{x} = \sum_{i=0}^{n} \frac{x_i}{\sum_{j=0}^{j=1}r_j} \;\;\; \text{num}_{y} = \sum_{i=0}^{n} \frac{y_i}{\sum_{j=0}^{j=1}r_j} \;\;\; \text{num}_{y} = \sum_{i=0}^{n} \frac{1}{\sum_{j=0}^{j=1}r_j}" />
</div>

<div align="center">
<img src="https://latex.codecogs.com/gif.latex?\inline&space;\dpi{300}&space;\small&space;\text{result\_coordinates}&space;=&space;(\frac{\text{num}_{x}}{\text{den}},&space;\frac{\text{num}_{y}}{\text{den}})" title="\small \text{result\_coordinates} = (\frac{\text{num}_{x}}{\text{den}}, \frac{\text{num}_{y}}{\text{den}})" />
</div>


### Analisi

- Statistica di Lungo Periodo
- Analisi MAC nascosti
- Visualizzazione Movimento

## Sviluppi Futuri

*Work in Progress...*
