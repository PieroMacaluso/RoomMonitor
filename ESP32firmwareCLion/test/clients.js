/**
 * Start this client with `node client.js`
 * This client sends data to your RoomMonitor for test purpouse
 */

/**
 * Parametro Numero di schedine
 * @type {number}
 */
const schedine = 4;

/**
 * Funzione di sleep
 * TODO: Implementare invio dei pacchetti ogni X minuti
 * @param ms
 * @returns {Promise<*>}
 */
async function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

/**
 * Funzione che genera un ID esadecimale di lunghezza definita
 * @param length
 * @returns {string|string}
 */
function makeid(length) {
    var result = '';
    var characters = 'abcdef0123456789';
    var charactersLength = characters.length;
    for (var i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

/**
 * Generatore Mac Casuale
 * @returns {string}
 */
function makeMac() {
    return "XX:XX:XX:XX:XX:XX".replace(/X/g, function () {
        return "0123456789ABCDEF".charAt(Math.floor(Math.random() * 16))
    });
}


/**
 * Main Function
 */

var net = require('net');
// Array dei client TCP
var client = [];
// Array dei pacchetti per ogni client  TCP
var packets = [];
// Posizione delle schedine
var posSchedine = [[0, 0], [0, 1], [1, 0], [1, 1]];


// Generazione causale di 4 pacchetti, il 5 sarà l'unico che dovrà risultare dall'aggregazione in generale
for (let i = 0; i < schedine; i++) {
    packets[i] = '';
    packets[i] += i + ',' + posSchedine[i][0] + ',' + posSchedine[i][1] + ',' + makeid(8) + ',' + '-30,' + makeMac() + ',0,PieroTest,cia;';
    packets[i] += i + ',' + posSchedine[i][0] + ',' + posSchedine[i][1] + ',' + makeid(8) + ',' + '-30,' + makeMac() + ',0,PieroTest,cia;';
    packets[i] += i + ',' + posSchedine[i][0] + ',' + posSchedine[i][1] + ',' + makeid(8) + ',' + '-30,' + makeMac() + ',0,PieroTest,cia;';
    packets[i] += i + ',' + posSchedine[i][0] + ',' + posSchedine[i][1] + ',' + makeid(8) + ',' + '-30,' + makeMac() + ',0,PieroTest,cia;';
    packets[i] += i + ',' + posSchedine[i][0] + ',' + posSchedine[i][1] + ',' + '11111111' + ',' + '-30,' + '22:22:22:22:22:22' + ',0,PieroTest,cia;';
    console.log(packets[i]);

}

// Creazione client TCP
for (let i = 0; i < schedine; i++) {
    client[i] = new net.Socket();
}

// Invio dati da ogni client TCP
for (let i = 0; i < schedine; i++) {
    console.log("Start " + i);
    client[i].connect(27015, '127.0.0.1', function () {
        console.log('Connected');
        // Insert the data you prefer
        client[i].write(packets[i]);
    });

    client[i].on('data', function (data) {
        console.log('Received: ' + data);
        client[i].destroy(); // kill client[i].after server's response
    });

    client[i].on('close', function () {
        console.log('Connection closed');
    });

    client[i].on('error', function () {
        console.log('Connection error: the server is unavailable!');
    });
}