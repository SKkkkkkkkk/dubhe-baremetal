#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	puts("StackOverflow\n\r");
	while (1)
		__asm__ volatile("nop");
}

void vApplicationMallocFailedHook( void )
{
	puts("MallocFailed\n\r");
	while (1)
		__asm__ volatile("nop");
}