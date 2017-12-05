#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 1024
#define EXPORT_FILE "/sys/class/gpio/export"

int readFileToScreen(char* filename);


void writeToFile(int pin, char* value);


void gpioPinInit(int pin, char* direction);

#endif