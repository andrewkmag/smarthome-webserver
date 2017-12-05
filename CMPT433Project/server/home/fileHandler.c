#include "fileHandler.h"

int readFileToScreen(char* filename)
{
	FILE* file = fopen(filename,"r");
	if(!file)
	{
		printf("ERROR: Unable to open file (%s) for read\n",filename);
		exit(-1);
	}
	char buff[BUFF_SIZE];
	fgets(buff,BUFF_SIZE,file);

	fclose(file);
	int ret = atoi(buff);
	return ret;
}


void writeToFile(int pin, char* value)
{
	char buff[BUFF_SIZE];
	sprintf(buff,"/sys/class/gpio/gpio%d/value", pin);
	FILE* pfile = fopen(buff, "w");
	if(!pfile)
	{
		printf("ERROR: Unable to open file! LINE 70\n");
		exit(1);
	}

	fprintf(pfile, "%s", value);
	fclose(pfile);
}

void gpioPinInit(int pin, char* direction)
{
	char buff[BUFF_SIZE];

	//add gpio pin to export file
	FILE* pExportFile = fopen(EXPORT_FILE,"w");
	if(!pExportFile)
	{
		perror("Line 29 ERROR: ");
		exit(1);
	}
	fprintf(pExportFile, "%d", pin);
	fclose(pExportFile);

	//set direction of pin based on direction parameter
	sprintf(buff,"/sys/class/gpio/gpio%d/direction", pin);
	FILE* pDirectionFile = fopen(buff, "w");
	if(!pDirectionFile)
	{
		perror("Line 40 ERROR: ");
		exit(1);
	}
	fprintf(pDirectionFile, "%s", direction);
	fclose(pDirectionFile);
}