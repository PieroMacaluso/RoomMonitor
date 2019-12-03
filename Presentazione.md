# Schema Presentazione Finale

1. ESP32 Firmware
   1. Acquisizione dei Pacchetti PROBE REQUEST
      1. Tempistiche
      2. Filtraggio dei pacchetti
      3. Storage in lista linkata liberata ogni minuto
   2. Setup della schedina
      1. HTTPS Server (NVS, SPIFFS) per Captive Portal
      2. TCP Socket connessione al Server e HMAC
   3. Sicurezza
      1. HMAC-SHA256 e discussione su HTTPS
2. RoomMonitor App
   1. Impostazioni
      1. Descrizione Generale
      2. Possibilità di gestire più stanze
   2. Ricezione
      1. Gestione Asincrona
      2. Coda di pacchetti Double Chance
   3. Aggregazione
      1. Ricezione da tutte le schedine
      2. Da RSSI a posizione
   4. Presentazione dei Dati
      1. Query, con riferimenti particolari alle granularità fatte con SQL
      2. Grafici con Callout
   5. MAC randomico
      1. Scelte effettuate
   6. Logger

## Domande

- Perchè C e non C++?
- 