# Guida avvio progetto QT
## Steps
- [Installazione QT](#installazione-qt)
- [Installazione MariaDB](#installazione-mariadb)
- [Creazione DB in MariaDB](#creazione-db-in-mariadb)
- [Apertura progetto](apertura-progetto)
## Installazione QT
1. Andare su sulla [pagina di download](https://www.qt.io/download) di QT
2. Scorrere in basso e premere su **Go Open Source** e infine du **Download**
3. Installare il programma
4. **(Opzionale)** Creazione Account QT
5. Conclusa installazione con successo!

**ATTENZIONE**: In caso di problemi nello scaricamento e/o nell'installazione si consiglia di scaricare tutto il pacchetto
per l'installazione offline al seguente [LINK](https://download.qt.io/static/mirrorlist/).

## Installazione MariaDB
1. Andare su [MariaDB releases](https://downloads.mariadb.org/mariadb/+releases/) di QT
2. Selezionare l'ultima versione che riporta un *Release Status* uguale a **Stable** (ad oggi 10.3.15)
3. Selezionare la versione per windows con nome tipo `mariadb-10.3.15-winx64.msi`
4. Premere su **No thanks, just take me to the download**
5. Avviare l'installazione e procedere fino a quando non vi chiederà la **root password**.
6. Attivare le spunte su **Enable access form remote machines** e **Use UTF8 as default server's character set**
7. Inserire la password due volte. Per il nostro progetto utilizzeremo: `NewRoot12Kz`
8. Andare avanti nell'installazione, premere su Install e il gioco è fatto.

## Creazione DB in MariaDB
1. Aprire il programma **HeidiSql**
2. Premere su *Nuovo* in basso a sinistra e inserire un nome a piacimento (e.g. `PDS`)
3. Inserire i seguenti dati nel form a destra:
    - **Hostname/IP**: `localhost`
    - **User**: `root`
    - **Password**: `NewRoot12Kz`
4. Premere su **Open**
5. Premere a sinistra su **PDS** (icona con foca e stella) e successivamente premere sulla tab **Query** nella parte destra.
6. Scrivere la query:
```sql
CREATE DATABASE data;
```
7. Avviare la query premendo il pulsante (simile a :arrow_forward: ) blu nella toolbar.
8. Aggiornare premendo con il tasto destro su PDS e successivamente su aggiorna.
9. Selezionare la tabella `data` appena creata con un doppio click e lanciare la query:
```sql
CREATE TABLE campi(
    id int(11) NOT NULL,
    hash varchar(8) NOT NULL,
    rssi int(11) NOT NULL,
    mac varchar(17) NOT NULL,
    timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    ssid varchar(64) NOT NULL,
    board varchar(17) NOT NULL);
```
10. Aggiornare nuovamente e verificare che tutto sia andato a buon fine.
11. **TODO**: script popolamento di test

## Apertura progetto
1. Avviare **Qt Creator** e aprire il file del progetto `funz.pro`. Oppure aprire direttamente il file `funz.pro`.
2. Premere sul tasto Play Verde in basso a Sinistra.
3. Chiudere subito il programma, anche se continuate verrete fermati da un messaggio d'errore tipo `DRIVER NOT LOADED`
4. Recatevi nella cartella del progetto e copiate il file `libmysql.dll` all'interno della cartella del tipo `build-*` che si sarà creata.
5. Riprovate ad avviare il programma in Qt Creator
6. Stavolta il programma dovrebbe aprirsi senza problemi.
