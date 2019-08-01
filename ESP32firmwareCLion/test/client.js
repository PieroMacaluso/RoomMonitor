/**
 * Start this client with `node client.js`
 * This client sends data to your RoomMonitor for test purpouse
 */

var net = require('net');

var client = new net.Socket();
client.connect(27015, '127.0.0.1', function() {
    console.log('Connected');
    // Insert the data you prefer
    client.write('1,1,1,1a2d3e52,-30,22:22:22:22:22:22,0,PieroTest,cia;');
    client.write('1,1,1,1a2d3e52,-30,22:22:22:22:22:22,0,PieroTest,cia;');
});

client.on('data', function(data) {
    console.log('Received: ' + data);
    client.destroy(); // kill client after server's response
});

client.on('close', function() {
    console.log('Connection closed');
});