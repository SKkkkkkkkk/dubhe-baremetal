#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void task1(void* arg)
{
	(void)arg;
	double i = 0;
	while(1)
	{
		printf("task1: %d\n\r", (int)i++);
	}
}


void task2(void* arg)
{
	(void)arg;
	double i = 0;
	while(1)
	{
		printf("task2: %d\n\r", (int)i++);
	}
}