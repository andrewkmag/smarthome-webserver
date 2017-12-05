#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1800
#define A2D_MAX_READING 4096

double tempReading;

int tempFlag;

char retrunString;

double getTemp(void);

void closeTempThread(void);

// char* getTempReadingForUDP(void);

void* tempThread(void* arg);

#endif