// Incomplete implementation of an audio mixer. Search for "REVISIT" to find things
// which are left as incomplete.
// Note: Generates low latency audio on BeagleBone Black; higher latency found on host.
#include "audioMixer.h"

static snd_pcm_t *handle;

#define DEFAULT_VOLUME 100
#define DEFAULT_BPM 120
#define MAX_BPM 300
#define MIN_BPM 40
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample
// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.

// Playback threading
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;
static int volume = 0;
static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;
static _Bool playing = false;
int songStatus = 8;

void nanoSleepFunc(int a, int b)
{
	for (int i = 0; i < 1; i++) {
		long seconds = a;
		long nanoseconds = b;
		struct timespec reqDelay = {seconds, nanoseconds};
		nanosleep(&reqDelay, (struct timespec *) NULL);
	}
}

void initSongs()
{
	wavedata_t jingleBells;
	AudioMixer_readWaveFileIntoMemory(JINGLE_BELLS, &jingleBells);

	wavedata_t rudolphRegular;
	AudioMixer_readWaveFileIntoMemory(RUDOLPH_REGULAR, &rudolphRegular);

	wavedata_t whiteChristmas;
	AudioMixer_readWaveFileIntoMemory(WHITE_CHRISTMAS, &whiteChristmas);

	wavedata_t sleighRide;
	AudioMixer_readWaveFileIntoMemory(SLEIGH_RIDE, &sleighRide);

	wavedata_t mistleToe;
	AudioMixer_readWaveFileIntoMemory(MISTLE_TOE, &mistleToe);


	wavedata_t rudolphRemix;
	AudioMixer_readWaveFileIntoMemory(RUDOLPH_REMIX, &rudolphRemix);

	songList[0] = jingleBells;
	songList[1] = rudolphRegular;
	songList[2] = whiteChristmas;
	songList[3] = sleighRide;
	songList[4] = mistleToe;
	songList[5] = rudolphRemix;
}

// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30
typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;
static playbackSound_t soundBites[MAX_SOUND_BITES];



void AudioMixer_init(void)
{
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	for(int i = 0; i < MAX_SOUND_BITES; i++)
	{
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}



	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	// pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}


// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	assert(pSound);
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}
	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);
	
	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

	pthread_mutex_lock(&audioMutex);
	_Bool inserted = false;
	for(int i = 0;i < MAX_SOUND_BITES;i++)
	{
		
		if(soundBites[i].pSound == NULL)
		{
			soundBites[i].pSound = pSound;
			soundBites[i].location = 0;
			inserted = true;
			break;
		}
		
	}
	pthread_mutex_unlock(&audioMutex);
	if(!inserted) {
		fprintf(stderr, "ERROR: AudioMixer unable to start new sound: NO ROOM\n");
	}
	
}

void AudioMixer_cleanup(void)
{
	printf("playing audio...\n");

	// Stop the PCM generation thread
	// playing = true;
	// pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	// snd_pcm_drain(handle);
	printf("awefawef\n");
	// snd_pcm_close(handle);
	printf("hereeeeee\n");
	// Initialize the currently active sound-bites being played
	for(int i = 0; i < MAX_SOUND_BITES; i++)
	{
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}
	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	// free(playbackBuffer);
	// printf("after free\n");
	// // playbackBuffer = NULL;
	// printf("after null\n");
	// // AudioMixer_init();
	// printf("after init\n");
	printf("free...\n");
	// fflush(stdout);
}


int AudioMixer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

char* AudioMixer_getBeat(int beat_num)
{
	if (beat_num == 0)
	{
		songStatus = 0;
		return "Jingle Bells\n";
	} 
	else if (beat_num == 1)
	{
		songStatus = 1;
		return "Rudolph\n";
	}
	else if(beat_num == 2)
	{
		songStatus = 2;
		return "White Christmas\n";
	}
	else if(beat_num == 3)
	{
		songStatus = 3;
		return "Sleigh Ride\n";
	}
	else if(beat_num == 4)
	{
		songStatus = 4;
		return "Mistle Toe\n";
	}
	else if(beat_num == 5)
	{
		songStatus = 5;
		return "Rudolph Remix\n";
	}
	else
	{
		return "None\n";
	}
}

static void fillPlaybackBuffer(short *playbackBuffer, int size)
{
	
	// clear PCM buffer
	memset(playbackBuffer,0,size*sizeof(*playbackBuffer));

	pthread_mutex_lock(&audioMutex);
	for(int i = 0; i < MAX_SOUND_BITES; i++)
	{
		wavedata_t *pSound = soundBites[i].pSound;
		if(pSound)
		{
			int samplei = soundBites[i].location;
			for(int j = 0; j < size; j++)
			{
				int newVal = (int) playbackBuffer[j] + pSound->pData[samplei];
				if(newVal > SHRT_MAX)
				{
					newVal = SHRT_MAX;
				}
				if(newVal < SHRT_MIN)
				{
					newVal = SHRT_MIN;
				}
				playbackBuffer[j] = (short) newVal;

				samplei++;

				if(samplei >= pSound->numSamples)
				{
					soundBites[i].pSound = NULL;
					samplei = 0;
					break;
				}
			}
			soundBites[i].location = samplei;
		}
	}
	pthread_mutex_unlock(&audioMutex);
}

void AudioMixer_reset(int newStatus)
{
	songStatus = newStatus; 
	// for(int i = 0; i < MAX_SOUND_BITES; i++)
	// {
	// 	soundBites[i].location = soundBites[i].pSound->numSamples;
	// }
}


void* playbackThread(void* arg)
{
	while (!playing) {
		printf("PLAYING...\n");
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,playbackBuffer, playbackBufferSize);
		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize) {
			printf("Short write (expected %li, wrote %li)\n",
					playbackBufferSize, frames);
		}
	}
	printf("NOT PLAYING....\n\n");
	return NULL;
}