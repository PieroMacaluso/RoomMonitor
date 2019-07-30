// ConsoleApplication1.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include "stdafx.h"
#include "pch.h"

#include "Aggregazione.h"
#include <iostream>
#include <thread>  
#include <deque> 

int main()
{//async ??
	Aggregazione aggrega;
	int n;
	deque <dati> deq;
	//std::cout << "Hello World!\n"; 
	//std::thread first(&aggrega);
	std::thread first(&Aggregazione::leggi, &aggrega,  std::ref(n), std::ref(deq));
	first.join();
}

// Per eseguire il programma: CTRL+F5 oppure Debug > Avvia senza eseguire debug
// Per eseguire il debug del programma: F5 oppure Debug > Avvia debug

// Suggerimenti per iniziare: 
//   1. Usare la finestra Esplora soluzioni per aggiungere/gestire i file
//   2. Usare la finestra Team Explorer per connettersi al controllo del codice sorgente
//   3. Usare la finestra di output per visualizzare l'output di compilazione e altri messaggi
//   4. Usare la finestra Elenco errori per visualizzare gli errori
//   5. Passare a Progetto > Aggiungi nuovo elemento per creare nuovi file di codice oppure a Progetto > Aggiungi elemento esistente per aggiungere file di codice esistenti al progetto
//   6. Per aprire di nuovo questo progetto in futuro, passare a File > Apri > Progetto e selezionare il file con estensione sln
