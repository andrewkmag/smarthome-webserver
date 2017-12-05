#include "homeserver.h"
#include "songs.h"

int main()
{

	AudioMixer_init();

	pthread_t serverID;
	pthread_t lockID;
	pthread_t lightID;
	pthread_t tempID;
	pthread_t songID;
	pthread_t playbackID;

	
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);

	pthread_create(&serverID, &attributes, server_thread, NULL);
	pthread_create(&lockID, &attributes, lockThread, NULL);
	pthread_create(&lightID, &attributes, lightThread, NULL);
	pthread_create(&tempID, &attributes, tempThread, NULL);
	pthread_create(&playbackID, &attributes, playbackThread, NULL);
	pthread_create(&songID, &attributes, songsThread, NULL);

	pthread_join(serverID, NULL);
	pthread_join(lockID, NULL);
	pthread_join(lightID, NULL);
	pthread_join(tempID, NULL);
	pthread_join(playbackID, NULL);
	pthread_join(songID, NULL);
	return 0;
}
