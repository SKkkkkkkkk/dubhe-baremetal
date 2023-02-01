#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "irq_ctrl.h"
#ifndef QEMU
	#include "system_counter.h"
#endif

int main()
{
#ifndef QEMU
	initSystemCounter(0, 0);
#endif
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