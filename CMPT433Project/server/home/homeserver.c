#include "homeserver.h"

_Bool stopping = false;
int current_light_val = 0;
// If current_lock_val = 1 == UNLOCK 
// current_lock_val = 0 == LOCK
int current_lock_val = 0;
char* room_views[3] = {"living", "kitchen", "master-bed"};
char* current_room_view;
int current_song = 8;

void set_current_light_val(int new_val)
{
	current_light_val = new_val;
}

void set_current_lock_val(int new_val)
{
	current_lock_val = new_val;
}

void set_room_view(int new_view)
{
	current_room_view = room_views[new_view];
}

int get_current_light_val()
{
	return current_light_val;
}

int get_current_lock_val()
{
	return current_lock_val;
}

int get_current_song()
{
	return current_song;
}

void set_current_song(int songNum)
{
	current_song = songNum;
}

char* get_room_view()
{
	if(current_room_view)
	{
		return current_room_view;
	}
	else
	{
		set_room_view(0);
		return current_room_view;
	}
	
}




char* command_light_handler(char* command_data)
{
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);

	if(strcmp(command_data, "1") == 0)
	{
		/*
		 *  TOGGLE LIGHT ON
		 */
		setLightState(1);
		char buff[4];
		sprintf(buff, "%d", 1);
		strcat(reply_msg,buff);
		set_current_light_val(getLightState());
		return reply_msg;
	}
	else if(strcmp(command_data, "0") == 0)
	{
		/*
		 *  TOGGLE LIGHT OFF
		 */
		setLightState(0);
		char buff[4];
		sprintf(buff, "%d", 0);
		strcat(reply_msg,buff);
		set_current_light_val(getLightState());
		return reply_msg;
	}
	else
	{
		// printf("ERROR in light parsing??\n");
		char buff[4];
		sprintf(buff, "%d", get_current_light_val());
		strcat(reply_msg,buff);
		return reply_msg;
	}
}

char* command_lock_handler(char* command_data)
{
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);
	if(strcmp(command_data, "1") == 0)
	{
		/*
		 * 	LOCK
		 */
		setLockState(0);
		char buff[4];
		sprintf(buff, "%d", getLockState());
		strcat(reply_msg,buff);
		set_current_lock_val(getLockState());
		return reply_msg;
	}
	else if(strcmp(command_data, "0") == 0)
	{
		/*
		 * 	UNLOCK
		 */
		setLockState(1);
		char buff[4];
		sprintf(buff, "%d", getLockState());
		strcat(reply_msg,buff);
		set_current_lock_val(getLockState());
		return reply_msg;
	}
	else
	{
		// printf("ERROR in lock parsing??\n");
		char buff[4];
		sprintf(buff, "%d", getLockState());
		strcat(reply_msg,buff);
		return reply_msg;
	}
}

char* command_room_handler(char* command_data)
{
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);
	/*
	 *  UPDATE ROOM STATUS
	 */
	if(strcmp(command_data, "living") == 0)
	{
		char buff[512];
		sprintf(buff, "%s", "living");
		strcat(reply_msg,buff);
		set_room_view(0);
		return reply_msg;
	}
	else if(strcmp(command_data, "kitchen") == 0)
	{
		char buff[512];
		sprintf(buff, "%s", "kitchen");
		strcat(reply_msg,buff);
		set_room_view(1);
		return reply_msg;
	}
	else if(strcmp(command_data, "master-bed") == 0)
	{
		char buff[512];
		sprintf(buff, "%s", "master-bed");
		strcat(reply_msg,buff);
		set_room_view(2);
		return reply_msg;
	}
	else 
	{
		// printf("Error in ROOM parsing??\n");
		char buff[512];
		sprintf(buff, "%s", get_room_view());
		strcat(reply_msg,buff);
		return reply_msg;
	}
}

char* command_temp_handler() {
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);

	double temp_msg = getTemp();
	char buff[512];
	sprintf(buff, "%.2f", temp_msg);
	strcat(reply_msg, buff);
	// printf("GOT TEMP [%s]\n", reply_msg);
	return reply_msg;
}

char* command_song_handler(char* command_data) {
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);
	char buff[512];

	// printf("Song:[%s]\n", command_data);
	if(strcmp(command_data, "0") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(0);
		set_current_song(0);
		sprintf(buff, "%s", AudioMixer_getBeat(0));
		strcat(reply_msg, buff);
		return reply_msg;
	}
	else if(strcmp(command_data, "1") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(1);
		set_current_song(1);
		sprintf(buff, "%s", AudioMixer_getBeat(1));
		strcat(reply_msg, buff);
		return reply_msg;
	} 
	else if(strcmp(command_data, "2") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(2);
		set_current_song(2);
		sprintf(buff, "%s", AudioMixer_getBeat(2));
		strcat(reply_msg, buff);
		return reply_msg;
	} 
	else if(strcmp(command_data, "3") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(3);
		set_current_song(3);
		sprintf(buff, "%s", AudioMixer_getBeat(3));
		strcat(reply_msg, buff);
		return reply_msg;
	} 
	else if(strcmp(command_data, "4") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(4);
		set_current_song(4);
		sprintf(buff, "%s", AudioMixer_getBeat(4));
		strcat(reply_msg, buff);
		return reply_msg;
	}
	else if(strcmp(command_data, "5") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(5);
		set_current_song(5);
		sprintf(buff, "%s", AudioMixer_getBeat(5));
		strcat(reply_msg, buff);
		return reply_msg;
	} 
	else if(strcmp(command_data, "8") == 0)
	{
		AudioMixer_cleanup();
		AudioMixer_reset(8);
		set_current_song(8);
		sprintf(buff, "%s", AudioMixer_getBeat(8));
		strcat(reply_msg, buff);
		return reply_msg;
	} 
	AudioMixer_reset(get_current_song());
	sprintf(buff, "%s", AudioMixer_getBeat(get_current_song()));
	strcat(reply_msg, buff);
	// printf("here\n");
	return reply_msg;
}

char* command_invalid()
{
	char* reply_msg = (char *)malloc(sizeof(char)*REPLY_SIZE);
	memset(reply_msg,0,sizeof(char)*REPLY_SIZE);
	strcat(reply_msg,"Invalid command: Consult the \"help\" command\n");
	return reply_msg;
}

char* command_controller(char* command)
{
	char* command_name;
	char* command_data;
	char* parse_command;
	parse_command = strtok(command, " ");
	command_name = parse_command;
	// printf("Command Name: [%s]\n", command_name);

	while(parse_command != NULL)
	{
		command_data = parse_command;
		// printf("parse_command: [%s]\n", parse_command);
		parse_command = strtok(NULL, " ");
	}
	
	// printf("COMMAND_CONTROLLER Data: [%s]\n", command_data);

	if(strcmp(command_name, "light") == 0)
	{
		return command_light_handler(command_data);
	}
	if(strcmp(command_name, "lock") == 0)
	{
		return command_lock_handler(command_data);
	}
	if(strcmp(command_name, "room") == 0)
	{
		return command_room_handler(command_data);
	}
	if(strcmp(command_name, "temp") == 0)
	{
		// printf("TEMP HERE\n");
		return command_temp_handler();
	}
	if(strcmp(command_name, "sound") == 0)
	{
		return command_song_handler(command_data);
	}
	printf("ERROR in command_controller\n");
	return command_invalid();
}

void* server_thread(void* arg)
{
	// initializing varaibles
	char command[1024];
	int sock;
	struct sockaddr_in name;
	struct sockaddr_storage clientAddr;
	socklen_t client_size;
	int nBytes;

	// creating socket
	sock = socket(AF_INET, SOCK_DGRAM,0);
	if(sock < 0)
	{
		perror("Opening datagram socket\n");
		exit(1);
	}

	// initializing network information
	bzero((char *) &name, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port = htons(SERVER_PORT);

	// binding to a particular port, after port has been initialized
	if(bind(sock,(struct sockaddr *) &name, sizeof(name)))
	{
		perror("binding datagram socket\n");
		exit(1);
	}

	// size of the client's info
	client_size = sizeof(clientAddr);
	while((nBytes = recvfrom(sock,command,1024,0,(struct sockaddr *)&clientAddr,&client_size)) > 0
		&& !stopping)
	{
		command[nBytes] = '\0'; // set the last value in command to a null terminating character

		if(strcmp(command,"stop\n") == 0)
		{
			// printf("STOP COMMAND DETECTED!\n");
			stopping = true;
			break;
		}
		
		char* response = command_controller(command);
		
		sendto(sock,response,strlen(response),0,(struct sockaddr *)&clientAddr,client_size);
		free(response);
	}

	close(sock);
	pthread_exit(0);
}