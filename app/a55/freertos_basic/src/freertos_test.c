#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void task1(void* arg)
{
	(void)arg;
	double i = 3.1415926;
	while(1)
	{
		printf("task1: %f\n\r", (i++));
		// vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void task2(void* arg)
{
	(void)arg;
	double i = 3.1415926;
	while(1)
	{
		printf("task2: %f\n\r", (i++));
		// vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

