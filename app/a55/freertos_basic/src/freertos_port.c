#include "FreeRTOS.h"
#include "task.h"
#include "irq_ctrl.h"
#include "gic.h"
#include <arch_helpers.h>
#include <stdio.h>

void vApplicationFPUSafeIRQHandler(uint32_t ulICCIAR)
{
	uint32_t ulInterruptID;

	/* In the 64-bit Cortex-A RTOS port it is necessary to clear the source of
    the interrupt BEFORE interrupts are re-enabled. */
	//ClearInterruptSource();

	/* Re-enable interrupts. */
	portENABLE_INTERRUPTS();

	/* The ID of the interrupt can be obtained by bitwise ANDing the ICCIAR value
    with 0x3FF. */
	ulInterruptID = ulICCIAR & 0x3FFUL;

	/* On the assumption that handlers for each interrupt are stored in an array
    called InterruptHandlerFunctionTable, use the interrupt ID to index to and
    call the relevant handler function. */
	IRQHandler_t handler = IRQ_GetHandler(ulInterruptID);
	if (handler != NULL)
		handler();
}

void SystemSetupSystick(void)
{
#ifndef QEMU
	write_cntfrq_el0(configSYS_COUNTER_FREQ_IN_TICKS);
#endif
	write_cntps_ctl_el1(0);
	IRQ_SetHandler(Secure_physical_timer_IRQn, FreeRTOS_Tick_Handler); //设置中断处理函数
	IRQ_SetPriority(Secure_physical_timer_IRQn, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT); //设置优先级
	IRQ_Enable(Secure_physical_timer_IRQn); //使能该中断

	write_cntps_tval_el1(read_cntfrq_el0()/configTICK_RATE_HZ);
	write_cntps_ctl_el1(1);
}

void SystemClearSystickFlag(void)
{
	write_cntps_tval_el1(read_cntfrq_el0()/configTICK_RATE_HZ);
}



#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

	/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
	implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
	used by the Idle task. */
	void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer,
									uint32_t *pulIdleTaskStackSize )
	{
		/* If the buffers to be provided to the Idle task are declared inside this
		function then they must be declared static - otherwise they will be allocated on
		the stack and so not exists after this function exits. */
		static StaticTask_t xIdleTaskTCB;
		static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

		/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
		state will be stored. */
		*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

		/* Pass out the array that will be used as the Idle task's stack. */
		*ppxIdleTaskStackBuffer = uxIdleTaskStack;

		/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
		Note that, as the array is necessarily of type StackType_t,
		configMINIMAL_STACK_SIZE is specified in words, not bytes. */
		*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	}
	/*-----------------------------------------------------------*/

	/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
	application must provide an implementation of vApplicationGetTimerTaskMemory()
	to provide the memory that is used by the Timer service task. */
	void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
										StackType_t **ppxTimerTaskStackBuffer,
										uint32_t *pulTimerTaskStackSize )
	{
		/* If the buffers to be provided to the Timer task are declared inside this
		function then they must be declared static - otherwise they will be allocated on
		the stack and so not exists after this function exits. */
		static StaticTask_t xTimerTaskTCB;
		static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

		/* Pass out a pointer to the StaticTask_t structure in which the Timer
		task's state will be stored. */
		*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

		/* Pass out the array that will be used as the Timer task's stack. */
		*ppxTimerTaskStackBuffer = uxTimerTaskStack;

		/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
		Note that, as the array is necessarily of type StackType_t,
		configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
		*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
	}
#endif


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