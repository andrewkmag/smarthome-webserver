#ifndef _HOMESERVER_H_
#define _HOMESERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "relayHandler.h"
#include "tempSensor.h"
#include "audioMixer.h"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define SERVER_PORT 12345
#define REPLY_SIZE 512

// Global variables:
_Bool stopping;
int current_light_val;
int current_lock_val;
char* room_views[3];
char* current_room_view;

// Getter Functions:
int get_current_light_val();
int get_current_lock_val();
char* get_room_view();

// Setter Functions:
void set_current_light_val(int new_val);
void set_current_lock_val(int new_val);
void set_room_view(int new_view);

// Command Function Handlers:
char* command_temp_handler();
char* command_room_handler(char* command_data);
char* command_light_handler(char* command_data);
char* command_lock_handler(char* command_data);
char* command_invalid();
char* command_controller(char* command);

// UDP connection thread:
void* server_thread(void* arg);

#endif