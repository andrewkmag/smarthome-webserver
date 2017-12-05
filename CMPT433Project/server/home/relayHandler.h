#ifndef RELAYHANDLER_H_
#define RELAYHANDLER_H_

#include <time.h>
#include <pthread.h>

#define IN "in"
#define OUT "out"

#define ON "1"
#define OFF "0"

#define LIGHT_RELAY_CONTROL_PIN 44 // P8_7 on beaglebone
#define LOCK_RELAY_CONTROL_PIN 65 // P8_10 on beaglebone

#define BUFF_SIZE 1024

int lockFlag;

int lightFlag;

int lockState;

int lightState;

void nanoSleep(int a, int b);

int getLockState();

int getLightState();

void setLockState(int aState);

void setLightState(int aState);

void setLock_ON_OR_OFF(int aState);

void setLight_ON_OR_OFF(int aState);

void* lockThread(void* arg);

void* lightThread(void* arg);

#endif