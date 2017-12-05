#include "relayHandler.h"
#include "fileHandler.h"

int lockFlag = 1;

int lightFlag = 1;

int lockState = 0;

int lightState = 0;

void nanoSleep(int a, int b)
{
	long seconds = a;
	long nanoseconds = b;
	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}

int getLockState()
{
	return lockState;
}

int getLightState()
{
	return lightState;
}

void setLockState(int aState)
{
	lockState = aState;
}

void setLightState(int aState)
{
	lightState = aState;
}

void setLock_ON_OR_OFF(int aState)
{
	if(aState == 0)
	{
		if(!getLockState())
			writeToFile(LOCK_RELAY_CONTROL_PIN, ON);
	}
	else
	{
		if(getLockState())
			writeToFile(LOCK_RELAY_CONTROL_PIN, OFF);
	}
}

void setLight_ON_OR_OFF(int aState)
{
	if(aState)
	{
		if(getLightState())
			writeToFile(LIGHT_RELAY_CONTROL_PIN, ON);
	}
	else
	{
		if(!getLightState())
			writeToFile(LIGHT_RELAY_CONTROL_PIN, OFF);
	}
}

void* lockThread(void* arg)
{
	gpioPinInit(LOCK_RELAY_CONTROL_PIN, OUT);

	while(lockFlag)
	{
		setLock_ON_OR_OFF(getLockState());
		nanoSleep(0, 500000000);
	}

	pthread_exit(0);
}

void* lightThread(void* arg)
{
	gpioPinInit(LIGHT_RELAY_CONTROL_PIN, OUT);

	while(lightFlag)
	{
		setLight_ON_OR_OFF(getLightState());
		nanoSleep(0,500000000);
	}

	pthread_exit(0);
}