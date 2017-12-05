"use strict";

// Web sockets: automatically establish a socket with the server
var socket = io.connect();
// light value of 1 == to light is ON, 0 == light is OFF
var light = 0;
// lock value of 1 == UNLOCKED, 0 == LOCKED
var lock = 0;
var connectionTimeout = null;
var hideErrorTimeout;
var dashboardEnabled = false;
var isLocked = true;
var songNum = 0;
var isSongPlaying = true;
var lastSongNum = 0;

$(document).ready(function() {

	serverMessageHandler(socket);

	// Setup a repeating function (every 1s)
	window.setInterval(function() {sendCommand("light")}, 1000);
	window.setInterval(function() {sendCommand("lock")}, 1000);
	window.setInterval(function() {sendCommand("room")}, 1000);
	window.setInterval(function() {sendCommand("temp")}, 1000);
	window.setInterval(function() {sendCommand("sound")}, 1000);

	// "Poll" the light, lock, room and temp:
	sendCommand("light");
	sendCommand("lock");
	sendCommand("room");
	sendCommand("temp");
	sendCommand("sound");


	// Buttons handling Current Room Commands
	$('#roomLiving').click(function() {
		// room = "living";
		sendCommand("room", "living");
	});
	$('#roomKitchen').click(function() {
		// room = "kitchen";
		sendCommand("room", "kitchen");
	});
	$('#roomMasterBed').click(function() {
		// room = "master-bed";
		sendCommand("room", "master-bed");
	});

	// Buttons handling Light Commands
	$('#lightSlider').click(function(){
		if($('#lightSlider').is(":checked")) {
			light = 1;
			sendCommand("light", light);
		} else {
			light = 0;
			sendCommand("light", light);
		}
	});

	// Buttons handling Lock Commands
	$('#lockSwitch').click(function() {
		if(isLocked == true) {
			// LOCKED
			lock = 0;
			isLocked = false;
			sendCommand("lock", lock);
		} else if(isLocked == false) {
			// UNLOCKED
			lock = 1;
			isLocked = true;
			sendCommand("lock", lock);
		}
	});

	// Buttons handling Music Commands
	$('#songDown').click(function() {
		songNum--;
		if(songNum > 5) {
			songNum = 0;
			lastSongNum = 0;
			sendCommand("sound", songNum);
		} else if(songNum < 0) {
			songNum = 5;
			lastSongNum = 5;
			sendCommand("sound", songNum);
		} else {
			lastSongNum = songNum;
			sendCommand("sound", songNum);
		}
	});

	$('#playSong').click(function() {
		songNum = 0;
		sendCommand("sound",songNum);
	});

	$('#stopSong').click(function() {
		songNum = 8;
		sendCommand("sound",songNum);
	});

	$('#songUp').click(function() {
		songNum++;
		if(songNum > 5) {
			songNum = 0;
			lastSongNum = 0;
			sendCommand("sound", songNum);
		} else if(songNum < 0) {
			songNum = 0;
			lastSongNum = 0;
			sendCommand("sound", songNum);
		} else {
			lastSongNum = songNum;
			sendCommand("sound", songNum);
		}
	});

	// HTML related functionality
	$('#dashButton').click(function() {
		if(dashboardEnabled == true) {
			closeDashboard();
			dashboardEnabled = false;
		} else {
			openDashboard();
			dashboardEnabled = true;
		}
	});

});

function sendCommand(command, update) {
	if(connectionTimeout == null) {
		connectionTimeout = setTimeout(errorHandler, 1000,
			"ERROR: Unable to communicate to server. Is nodeJS server running?");
	}
	socket.emit(command, update);
}

function resetServerTimeout() {
	clearTimeout(connectionTimeout);
	connectionTimeout = null;
}

function errorHandler(message) {
	console.log("ERROR Handler: " + message);

	$('#error-text').html(message);
	$('#error-box').show(); //.show() initially

	// Hide error after some time
	window.clearTimeout(hideErrorTimeout);
	hideErrorTimeout = window.setTimeout(function() {$('#error-box'),hide();}, 5000);
	resetServerTimeout();
}

function serverMessageHandler(socket) {
	// Initially hide error display:
	$('#error-box').hide();

	socket.on('roomStatus', function(result) {
		console.log("Status Update: roomStatus " + result);
		var room = "unknown";
		switch(result)
		{
			case "living": room = "Living Room"; break;
			case "kitchen": room = "Kitchen"; break;
			case "master-bed": room = "Master Bedroom"; break;
		}

		$('#roomID').text(room);
		
		if (result == "living" && light == 1) {
			$('#lightSlider').show();
			$('#sliderClass').show();
			$('#lockSwitch').show();
			toggleLivLit();
		} else if (result == "living" && light == 0) {
			$('#lightSlider').show();
			$('#sliderClass').show();
			$('#lockSwitch').show();
			toggleLivDark();
		} else if (result == "kitchen") {
			$('#lightSlider').hide();
			$('#sliderClass').hide();
			$('#lockSwitch').hide();
			toggleKitchenDark();
		} else if (result == "master-bed") {
			$('#lightSlider').hide();
			$('#sliderClass').hide();
			$('#lockSwitch').hide();
			toggleBedDark();
		}
		resetServerTimeout();
	});

	socket.on('lightStatus', function(result) {
		console.log("Status Update: lightStatus " + result);
		$('#lightVal').val(result);
		if(result == 1)
		{
			$('#lightSlider').prop("checked", true);
		} else {
			$('#lightSlider').prop("checked", false);
		}
		resetServerTimeout();
	});

	socket.on('lockStatus', function(result) {
		var flip = result;
		if (result == 1) {
			flip = 0;
			$('#lock-text').text(' LOCKED');
		} else {
			flip = 1;
			$('#lock-text').text(' UNLOCKED');
		}
		console.log("Status Update: lockStatus " + flip);

		resetServerTimeout();
	});

	socket.on('tempStatus', function(result) {
		var result = result + "&#8451;";
		$('#temperatureDisplayDiv').html(result);
	});

	socket.on('soundStatus', function(result) {
		// var result = "Jingle Bells";
		$('#musicDisplay').html(result);
	});

	socket.on('connectionError', errorHandler);
}

function openDashboard() {
	document.getElementById("dashID").style.width = "250px";
	document.getElementById("content").style.marginLeft = "250px";
}
function closeDashboard() {
	document.getElementById("dashID").style.width = "0px";
	document.getElementById("content").style.marginLeft = "0px";
}

function toggleLivLit() {
	if(($('#room-pictureID').hasClass('livdark-img')) == true) {
		$('#room-pictureID').removeClass('livdark-img');
	}

	if(($('#room-pictureID').hasClass('kitchen-dark-img')) == true) {
		$('#room-pictureID').removeClass('kitchen-dark-img');
	}

	if(($('#room-pictureID').hasClass('bed-dark-img')) == true) {
		$('#room-pictureID').removeClass('bed-dark-img');
	}

	if(($('#room-pictureID').hasClass('livlit-img')) == true) {
		// do nothing
	} else {
		$('#room-pictureID').addClass('livlit-img');
	} 
}

function toggleLivDark() {
	if(($('#room-pictureID').hasClass('livlit-img')) == true) {
		$('#room-pictureID').removeClass('livlit-img');
	} 

	if(($('#room-pictureID').hasClass('kitchen-dark-img')) == true) {
		$('#room-pictureID').removeClass('kitchen-dark-img');
	}

	if(($('#room-pictureID').hasClass('bed-dark-img')) == true) {
		$('#room-pictureID').removeClass('bed-dark-img');
	}

	if(($('#room-pictureID').hasClass('livdark-img')) == true) {
		// do nothing
	} else {
		$('#room-pictureID').addClass('livdark-img');
	} 
}

function toggleKitchenDark() {
	if(($('#room-pictureID').hasClass('livlit-img')) == true) {
		$('#room-pictureID').removeClass('livlit-img');
	} 

	if(($('#room-pictureID').hasClass('livdark-img')) == true) {
		$('#room-pictureID').removeClass('livdark-img');
	}

	if(($('#room-pictureID').hasClass('bed-dark-img')) == true) {
		$('#room-pictureID').removeClass('bed-dark-img');
	}

	if(($('#room-pictureID').hasClass('kitchen-dark-img')) == true) {
		// do nothing
	} else {
		$('#room-pictureID').addClass('kitchen-dark-img');
	} 
}

function toggleBedDark() {
	if(($('#room-pictureID').hasClass('livlit-img')) == true) {
		$('#room-pictureID').removeClass('livlit-img');
	} 

	if(($('#room-pictureID').hasClass('livdark-img')) == true) {
		$('#room-pictureID').removeClass('livdark-img');
	}

	if(($('#room-pictureID').hasClass('kitchen-dark-img')) == true) {
		$('#room-pictureID').removeClass('kitchen-dark-img');
	}
	if(($('#room-pictureID').hasClass('bed-dark-img')) == true) {
		// do nothing
	} else {
		$('#room-pictureID').addClass('bed-dark-img');
	} 
}