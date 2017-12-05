#include "songs.h"

static _Bool streaming = false;
static int sleepTime = 1;
_Bool changedSong = false;

void jingleBells()
{
	AudioMixer_queueSound(&songList[0]);
	
}

void rudolphRegular()
{
	AudioMixer_queueSound(&songList[1]);
	
}

void whiteChristmas()
{
	AudioMixer_queueSound(&songList[2]);
	
}

void sleighRide()
{
	AudioMixer_queueSound(&songList[3]);
	
}

void mistleToe()
{
	AudioMixer_queueSound(&songList[4]);
	
}

void rudolphRemix()
{
	AudioMixer_queueSound(&songList[5]);
}


void songNone()
{
	return;
}

int adjustSleep() {
	if(songStatus == 0)
	{
		sleepTime = SONG_0_SLEEP_TIME;
	}
	else if(songStatus == 1)
	{
		sleepTime = SONG_1_SLEEP_TIME;
	}
	else if(songStatus == 2)
	{
		sleepTime = SONG_2_SLEEP_TIME;
	}
	else if(songStatus == 3)
	{
		sleepTime = SONG_3_SLEEP_TIME;
	}
	else if(songStatus == 4)
	{
		sleepTime = SONG_4_SLEEP_TIME;
	} 
	else if(songStatus == 5)
	{
		sleepTime = SONG_5_SLEEP_TIME;
	}
	else {
		sleepTime = 1;
	}
	return sleepTime;
}

void* songsThread(void* arg)
{
	initSongs();
	while(!streaming)
	{
		if(songStatus == 0)
		{
			jingleBells();
			// nanoSleepFunc(148,0);
		}
		if(songStatus == 1)
		{
			rudolphRegular();
			// nanoSleepFunc(190,0);
		}
		if(songStatus == 2)
		{
			whiteChristmas();
			// nanoSleepFunc(76,0);

		}
		if(songStatus == 3)
		{
			sleighRide();
			// nanoSleepFunc(175,0);

		}
		if(songStatus == 4)
		{
			mistleToe();
			// nanoSleepFunc(192,0);
		}
		if(songStatus == 5)
		{
			rudolphRemix();
			// nanoSleepFunc(95,0);
		}
		if(songStatus == 8)
		{
			songNone();
		}

		nanoSleepFunc(adjustSleep(),0);
	}

	pthread_exit(0);
}