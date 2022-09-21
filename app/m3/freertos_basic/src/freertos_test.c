#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void task1(void* arg)
{
	// static int i = 0;
	while(1)
	{
		printf("1\n\r");
		// vTaskDelay(1000);
	}
}


void task2(void* arg)
{
	static int i = 0;
	while(1)
	{
		printf("task2: %d\n\r", i++);
		vTaskDelay(1000);
	}
}
