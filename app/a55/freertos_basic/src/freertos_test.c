#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void task1(void* arg)
{
	static double i = 0;
	while(1)
	{
		printf("task1: %d\n\r", (int)(i++));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void task2(void* arg)
{
	static double i = 0;
	while(1)
	{
		printf("task2: %d\n\r", (int)(i++));
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

