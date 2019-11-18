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

- [Progetto Programmazione di Sistema](#progetto-programmazione-di-sistema)
  - [Indice](#indice)
  - [Introduzione](#introduzione)
  - [Team di sviluppo](#team-di-sviluppo)
  - [Componenti del progetto](#componenti-del-progetto)

## Introduzione

Questo progetto è stato sviluppato partendo dalla [proposta di progetto](stuff/specs.pdf) fornita dal prof. Giovanni Malnati durante il corso "Programmazione di Sistema" nell' a.a. 2017/2018. 

L'obiettivo principale del software implementato è quello di fornire un sistema capace di monitorare numero e posizione di studenti all'interno di un'aula. Questo è possibile sfruttando i pacchetti PROBE REQUEST utilizzati dai dispositivi che normalmente utilizziamo: questi vengono catturati da una serie di schedine ESP32 (2 o più) per poi essere trasmessi ad un server che si occupa di filtrare e aggregare i pacchetti ricevuti da tutte le schedine in azione per ottenere l'informazione sulla posizione partendo dai valori dell'RSSI.
Da questi valori è possibile passare ad un valore della distanza del singolo dispositivo (identificato dal suo MAC) da ogni schedina, utile per procedere con una triangolazione della posizione.

## Team di sviluppo

- **Piero Macaluso** - [pimack](https://github.com/pimack)
- **Lorenzo Manicone** - [lorenzomanicone](https://github.com/lorenzomanicone)
- **Angelo Turco** - [angeloturco](https://github.com/angeloturco)

## Componenti del progetto

- [**Firmware ESP Board**](ESP32firmwareCLion)
- [**Server App**](RoomMonitor)