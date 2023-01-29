#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "irq_ctrl.h"

int main()
{	
	IRQ_Initialize();
	void task1(void* arg);
	void task2(void* arg);
	if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	return 0;
}