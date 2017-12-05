#include "tempSensor.h"

double tempReading = 20.0;

int tempFlag = 1;

char returnString[16];

double getTemp(void)
{
	FILE* file = fopen(A2D_FILE_VOLTAGE1,"r");
	if(!file)
	{
		printf("ERROR: Unable to open voltage input file. Is Cape loaded?\n");
		printf("Try: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
		exit(1);
	}

	int reading = 0;
	int read = fscanf(file, "%d", &reading);
	if(read <= 0)
	{
		printf("ERROR: unable to read value from voltage input file.\n");
		exit(1);
	}

	fclose(file);

	return ((((double)reading/A2D_MAX_READING)*A2D_VOLTAGE_REF_V)-500)/10;
}

void closeTempThread(void)
{
	tempFlag = 0;
}

// char* getTempReadingForUDP(void)
// {
// 	memset(returnString, 0, 16);
// 	snprintf(returnString,16,"%.2f",tempReading);

// 	return returnString;
// }

void* tempThread(void* arg)
{
	while(tempFlag == 1)
	{
		tempReading = getTemp();
		sleep(1);
	}

	pthread_exit(0);
}
