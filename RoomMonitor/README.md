# RoomMonitor App

## Installing
Bisogna aprire la porta lato server per garantire un corretto funzionamento.

### Windows
  Creare una regola nel firewall per consentire la connessione in entrata tramite TCP sulla porta 27015.
  
### Linux

Per aprire la porta 27015 in entrata TCP su Linux usare il seguente comando:
```bash
sudo ufw allow 27015/tcp
```

Per controllare lo stato delle porte utilizzare il comando:

```bash
sudo ufw status
```