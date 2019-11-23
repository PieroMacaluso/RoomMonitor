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
- [Introduction](#introduction)
- [ESP32 Firmware](#esp32-firmware)
  - [Sniffing Pacchetti](#sniffing-pacchetti)
  - [Configurazione Schedina](#configurazione-schedina)
    - [Captive Portal](#captive-portal)
      - [NVS (Non Volatile Storage)](#nvs-non-volatile-storage)
      - [SPIFFS (Serial Peripheral Interface Flash File System)](#spiffs-serial-peripheral-interface-flash-file-system)
    - [](#)
- [RoomMonitor Server](#roommonitor-server)
  - [Generale](#generale)
  - [Impostazioni](#impostazioni)
  - [Monitoraggio](#monitoraggio)
  - [Analisi](#analisi)
- [Sviluppi Futuri](#sviluppi-futuri)
- [Team di sviluppo](#team-di-sviluppo)
- [Componenti del progetto](#componenti-del-progetto)

Questo progetto è stato sviluppato partendo dalla [proposta di progetto](stuff/specs.pdf) fornita dal prof. Giovanni Malnati durante il corso "Programmazione di Sistema" nell' a.a. 2017/2018. 

## Introduction

Ogni dispositivo WiFi, in particolare gli smartphone, possiedono la capacità di riconnettersi rapidamente ad una rete WiFi precedentemente associata non appena si vengono a trovare all'interno dell'area di copertura. Questo dispositi
Una delle caratteristiche	degli smartphone	è	la	loro	 capacità	di	 riconnettersi	rapidamente	a
una	 rete	 WiFi	 cui	 sono	 stati	 precedentemente	 associati,	 non	 appena	 si	 vengono	 a	 trovare	
all’interno	della	sua	area	di	copertura.	Tale	comportamento	è	reso	possibile	dalla	cosiddetta	
modalità	di	scansione	attiva,	per	la	quale,	invece	di	attendere	passivamente	la	ricezione	di	un	
pacchetto	 di	tipo	 **BEACON**	proveniente	dall’access	 point,	 essi	 inviano,	 ad	 intervalli	 regolari,
pacchetti	broadcast	di	tipo	**PROBE	REQUEST**,	eventualmente	indicando	il	nome	della	rete	a	cui	
essi	provano	a	connettersi.
Tale	 comportamento	 può	 essere	 utilizzato	 per	 realizzare	 un	 rilevatore	 di	 presenza,	 che	
determini,	in	una	data	area,	una	stima	del	numero	di	smartphone	presenti.

Il	dispositivo	ESP32	possiede un’implementazione	firmware	dello	stack	WiFi	che	permette	di	
registrare	una	callback		che	viene	invocata	ogniqualvolta	venga	ricevuto	un	pacchetto	di	tipo	
**CONTROL**,	 offrendo così	 la	 possibilità	 di	 rilevare	 sia	 la	 presenza	 di	 BEACON	 che	 di	 **PROBE	
REQUEST**.
Tale	callback	riceve	come	parametro	una	struttura	dati	che	fornisce,	oltre	al	pacchetto	stesso	
ricevuto,	anche	altri metadati,	tra	cui	la	potenza	del	segnale	ricevuto	(RSSI	– Received	Signal	
Strenght	 Indicator).	 In	 condizioni	 ideali,	 tale	 valore	 decresce	 esponenzialmente	 al	 crescere	
della	distanza	ed è pertanto	espresso	in	forma	logaritmica (dB).	Valori	tipici	variano	tra	-35/-
40	(molto vicino)	fino	a	-95/-105	(molto lontano,	ai	limiti	della	possibilità	di	ricezione).	Nella	
realtà,	vari fattori	possono	condizionare	tale	valore,	tra	cui	la	presenza	di	riflessioni	multiple	
dovute	 al	 terreno,	 ai	 muri	 e agli	 altri	 ostacoli	 alla	 propagazione	 o	 le	 interferenze	 con	 altri	
segnali	 presenti	 nell’ambiente.	 In	 ogni	 caso,	 esso	 fornisce	 una	 stima	 approssimata	 della	
distanza.

L'obiettivo principale del software implementato è quello di fornire un sistema capace di monitorare numero e posizione di studenti all'interno di un'aula. Questo è possibile sfruttando i pacchetti PROBE REQUEST utilizzati dai dispositivi che normalmente utilizziamo: questi vengono catturati da una serie di schedine ESP32 (2 o più) per poi essere trasmessi ad un server che si occupa di filtrare e aggregare i pacchetti ricevuti da tutte le schedine in azione per ottenere l'informazione sulla posizione partendo dai valori dell'RSSI.
Da questi valori è possibile passare ad un valore della distanza del singolo dispositivo (identificato dal suo MAC) da ogni schedina, utile per procedere con una triangolazione della posizione.

## ESP32 Firmware

### Sniffing Pacchetti

```c
// Conversione del buffer in pacchetto e estrazione di tipo e sottotipo
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *) buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *) ppkt->payload;
    const crc_t *crc = &ipkt->payload;
    time_t now;
    uint8_t frameControl = (unsigned int) ppkt->payload[0];
    uint8_t frameType = (frameControl & 0x0C) >> 2;
    uint8_t frameSubType = (frameControl & 0xF0) >> 4;

    // Filtraggio dei pacchetti non desiderati
    if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST) {
        return;
    }
```

`2,dc7ef681,-76,b4:f1:da:d9:2b:b2,1573214966,~,3C:71:BF:F5:9F:3C`
### Configurazione Schedina

#### Captive Portal

##### NVS (Non Volatile Storage)

Dati chiave valore.

##### SPIFFS (Serial Peripheral Interface Flash File System)

Storage HTML, CSS e JS della pagina di Captive Portal.

####  

- SPIFFS
- NTP
- TCP Socket
- Captive Portal
- Acquisizione Pacchetti
- Invio Pacchetti
- Validazione Input (Sviluppo Futuro)

## RoomMonitor Server

### Generale

![Interfaccia Principale]()

### Impostazioni

### Monitoraggio

- Ricezione
- Aggregazione (Double Chance, Triangolazione)
- Storage nel DB

### Analisi

- Statistica di Lungo Periodo
- Analisi MAC nascosti
- Visualizzazione Movimento

## Sviluppi Futuri



## Team di sviluppo

- <img alt="avatar" src="https://github.com/pieromacaluso.png" width="20px" height="20px"> **Piero Macaluso** - [pieromacaluso](https://github.com/pieromacaluso)
- <img alt="avatar" src="https://github.com/lorenzomanicone.png" width="20px" height="20px"> **Lorenzo Manicone** - [lorenzomanicone](https://github.com/lorenzomanicone)
- <img alt="avatar" src="https://github.com/angeloturco.png" width="20px" height="20px"> **Angelo Turco** - [angeloturco](https://github.com/angeloturco)

## Componenti del progetto

- [**Firmware ESP Board**](ESP32firmwareCLion)
- [**Server App**](RoomMonitor)
