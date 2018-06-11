# Server DB

## Breve guida per setup MySQL e librerie in VS

### Prerequisiti:

- [MySQL Workbench 8.0.11 rc ](https://dev.mysql.com/downloads/workbench/8.0.html)
- MySQL Connector/C++ 1.1.11
- MySQL Server 8.0.11

_Gli ultimi due possono essere installati attraverso [MySQL Installer](https://dev.mysql.com/downloads/installer/)_

###Accorgimenti installazione MySQL 

Annotare:

- User (_root_)
- Password
- Porta del database

In questa guida diamo per scontato l'installazione dei programmi nei prerequisiti!

### Setup Progetto VS

- Aprire il progetto in Visual Studio
- Impostare la configurazione **Debug** e **x64** in alto (x86 se la vostra versione di installazione dei tool MySQL fosse diversa)
- Aprire le **Proprietà del progetto** dal pannello **Esplora Soluzioni**
- Andare in **Proprietà di Configurazione** :arrow_forward: **C/C++** :arrow_forward: **Generale** :arrow_forward: **Directory di inclusione aggiuntive** e aggiungere la stringa `C:\Program Files\MySQL\Connector C++ 1.1\include;` o comunque quella relativa alla vostra installazione di *MySQL Connector/C++ 1.1.11* all'inizio
- Andare in **Proprietà di Configurazione** :arrow_forward: **Linker** :arrow_forward: **Generale** :arrow_forward: **Directory librerie aggiuntive** e aggiungere la stringa `C:\Program Files\MySQL\Connector C++ 1.1\lib\opt;` o comunque quella relativa alla vostra installazione di *MySQL Connector/C++ 1.1.11* all'inizio
- Andare in **Proprietà di Configurazione** :arrow_forward: **Linker** :arrow_forward: **Input** :arrow_forward: **Dipendenze aggiuntive**  e aggiungere `mysqlcppconn.lib;` all'inizio
- Copiare `C:\Program Files\MySQL\Connector C++ 1.1\lib\opt\mysqlcppconn.dll` all'interno della directory dove si trovano i nostri file di progetto cpp.
- Avvia! 