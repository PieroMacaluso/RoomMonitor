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
  - [Scansione Attiva](#scansione-attiva)
  - [Dispositivo ESP32](#dispositivo-esp32)
  - [Obiettivo del progetto](#obiettivo-del-progetto)
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
