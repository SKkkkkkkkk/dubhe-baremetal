#include "FreeRTOS.h"
#include "task.h"
// #include "ape1210.h"
#include "irq_ctrl.h"
#include "gic.h"
// #include "ape1210_timer.h"
// #include "seehi_print.h"
#include "dw_apb_timers.h"
#include <stdio.h>

void vApplicationIRQHandler(uint32_t ulICCIAR)
{
	uint32_t ulInterruptID;

	/* In the 64-bit Cortex-A RTOS port it is necessary to clear the source of
    the interrupt BEFORE interrupts are re-enabled. */
	//ClearInterruptSource();

	/* Re-enable interrupts. */
	// __asm volatile("CPSIE I");

	/* The ID of the interrupt can be obtained by bitwise ANDing the ICCIAR value
    with 0x3FF. */
	ulInterruptID = ulICCIAR & 0x3FFUL;

	/* On the assumption that handlers for each interrupt are stored in an array
    called InterruptHandlerFunctionTable, use the interrupt ID to index to and
    call the relevant handler function. */
	IRQHandler_t handler = IRQ_GetHandler(ulInterruptID);
	if (handler != NULL)
		handler();
	// IRQ_GetHandler(ulInterruptID)();
}

void SystemSetupSystick(void)
{
	timer_init_config_t timer_init_config = {
		.int_mask = 0, .loadcount = 20000000/configTICK_RATE_HZ, .timer_id = Timerx2_T1, .timer_mode = Mode_User_Defined
	};
	timer_init(&timer_init_config);

	void FreeRTOS_Tick_Handler(void);
	IRQ_SetHandler(Timerx2_T1_IRQn, FreeRTOS_Tick_Handler);
	IRQ_SetPriority(Timerx2_T1_IRQn, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT);
	IRQ_Enable(Timerx2_T1_IRQn);

	timer_enable(Timerx2_T1);
}

void SystemClearSystickFlag(void)
{
	(void)TIMERX2->Timer1EOI;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	// DI();
	// seehi_printf("StackOverflow!!! TCB: %lx, TaskName: %s\n\r", (uint32_t)xTask, pcTaskName);
	while (1)
		;
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

