# all: arm host

# arm:
# 	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  -L$(HOME)/cmpt433/public/asound_lib_BBB -lasound -lpthread -Werror homeserver.c homeserver.h fileHandler.c fileHandler.h relayHandler.c relayHandler.h tempSensor.c tempSensor.h audioMixer.c audioMixer.h songs.c songs.h main.c -o homeserver 
# 	cp homeserver $(HOME)/cmpt433/public/myApps/

# host:
# 	-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  -L$(HOME)/cmpt433/public/asound_lib_BBB -lasound -lpthread -Werror homeserver.c homeserver.h fileHandler.c fileHandler.h relayHandler.c relayHandler.h tempSensor.c tempSensor.h audioMixer.c audioMixer.h songs.c songs.h main.c -o homeserver
# 	cp hostv-homeserver $(HOME)/cmpt433/public/myApps/

TARGET= smarthome

SOURCES=homeserver.c homeserver.h fileHandler.c fileHandler.h relayHandler.c relayHandler.h tempSensor.c tempSensor.h audioMixer.c audioMixer.h songs.c songs.h main.c

PUBDIR= $(HOME)/cmpt433/public/myApps
OUTDIR= $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_C= $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS= -L$(HOME)/cmpt433/public/asound_lib_BBB

all: wav
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound

wav:
	mkdir -p $(PUBDIR)/songs/
	cp -R songs/* $(PUBDIR)/songs/ 	