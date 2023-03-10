#include "FreeRTOS.h"
#include "task.h"
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	while (1)
		__asm__ volatile("nop");
}

void vApplicationMallocFailedHook( void )
{
	while (1)
		__asm__ volatile("nop");
}