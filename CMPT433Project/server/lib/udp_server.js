// Disables creation of variables without declaration
"use strict";

/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */
var socketio = require('socket.io');
var io;
var dgram = require('dgram');

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');
	
	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {
	console.log("Setting up socket handler.");

	// Passed string of comamnd to relay
	socket.on("light", function(data) {
		console.log("Got light update command: " + data);
		relayToApp(socket, "light " + data, "lightStatus");
	});

	socket.on("lock", function(data) {
		console.log("Got lock update command: " + data);
		relayToApp(socket, "lock " + data, "lockStatus");
	});

	socket.on("room", function(data) {
		console.log("Got room update command: " + data);
		relayToApp(socket, "room " + data, "roomStatus");
	});

	socket.on("temp", function(data) {
		console.log("Got temp update command: " + data);
		relayToApp(socket, "temp " + data, "tempStatus");
	});

	socket.on("sound", function(data) {
		console.log("Got sound update command: " + data);
		relayToApp(socket, "sound " + data, "soundStatus");
	});
};

function relayToApp(socket, data, statusName) {
	// Info for connecting to the local process via UDP
		var PORT = 12345;
		var HOST = 'localhost';
		var buffer = new Buffer(data);

		// Send an error if we have not got a reply in a second
		var errorTimer = setTimeout(function() {
			console.log("ERROR: No reply from local application.");
			socket.emit("connectionError", "SERVER ERROR: No response from homeserver application. Is it running?");
		}, 1000);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});

		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8');

			socket.emit(statusName, reply);
		    client.close();

		});
		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
}