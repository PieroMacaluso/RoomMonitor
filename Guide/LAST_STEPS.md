# Last Steps

- [Last Steps](#last-steps)
  - [Team](#team)
  - [Avvio monitoraggio](#avvio-monitoraggio)
    - [Apertura](#apertura)
    - [Dati in ingresso](#dati-in-ingresso)
    - [Strutture dati](#strutture-dati)
  - [TODO](#todo)
    - [Ricezione](#ricezione)
      - [Comportamento Atteso](#comportamento-atteso)
    - [Aggregazione](#aggregazione)
      - [Comportamento Atteso](#comportamento-atteso-1)
    - [BONUS TRACK](#bonus-track)

## Team

- Lorenzo: 6-13 Agosto | 17-24 Agosto (basso regime)
- Angelo: Settembre disponibilità limitata
- Valentino: Settembre
- Piero: 29 Luglio - 6 Agosto (basso regime)

## Avvio monitoraggio

### Apertura

File -> Nuovo Monitoraggio | Pulsante nella toolbar

### Dati in ingresso

Numero schedine
(Dimensioni della stanza)

### Strutture dati

- Mutex
- Condition Variable
- Deque (push_back, front-pop_front FIFO), (push_back, elaborazione con algoritmi std e clear finale.)
- (Vector (push_back, elaborazione con algoritmi std e clear finale.))

## TODO

### Ricezione

**Input**: Dati schedina

**Output**: Deque di Packet dalle varie schedine

#### Comportamento Atteso

- Controllare i dati come vengono inviati da scheda (Telnet)
- New Connection elabori con thread e struttura condivisa

### Aggregazione

**Input**: Deque di Packet dalle varie schedine

**Output**: Stampa elenco MAC ricevuti da tutte le schedine | PASSAGGIO SUCCESSIVO: Inserimento nel database

#### Comportamento Atteso

- Presenza di thread temporizzato che sia avvia ogni minuto che svuota la coda

### BONUS TRACK

Ricerca per passare da RSSI a posizione X, Y
