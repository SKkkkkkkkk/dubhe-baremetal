#include "gic.h"
#include "irq_ctrl.h"
#include <stdio.h>

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

int main()
{
	GIC_Enable();
	printf("Generic Timer Test:\n\r");
	void EL3_physical_timer_delay1(void);
	void EL3_physical_timer_delay2(void);
	void EL3_physical_timer_delay3(void);
	void EL3_physical_timer_delay4(void);
	void Non_secure_EL2_physical_timer_delay1(void);
	void Non_secure_EL2_physical_timer_delay2(void);
	void Non_secure_EL2_physical_timer_delay3(void);
	void Non_secure_EL2_physical_timer_delay4(void);
	void EL1_physical_timer_delay1(void);
	void EL1_physical_timer_delay2(void);
	void EL1_physical_timer_delay3(void);
	void EL1_physical_timer_delay4(void);
	void EL1_virtual_timer_delay1(void);
	void EL1_virtual_timer_delay2(void);
	void EL1_virtual_timer_delay3(void);
	void EL1_virtual_timer_delay4(void);
	void Non_secure_EL2_virtual_timer_delay1(void);
	void Non_secure_EL2_virtual_timer_delay2(void);
	void Non_secure_EL2_virtual_timer_delay3(void);
	void Non_secure_EL2_virtual_timer_delay4(void);
	EL3_physical_timer_delay1();
	EL3_physical_timer_delay2();
	EL3_physical_timer_delay3();
	EL3_physical_timer_delay4();
	Non_secure_EL2_physical_timer_delay1();
	Non_secure_EL2_physical_timer_delay2();
	Non_secure_EL2_physical_timer_delay3();
	Non_secure_EL2_physical_timer_delay4();
	EL1_physical_timer_delay1();
	EL1_physical_timer_delay2();
	EL1_physical_timer_delay3();
	EL1_physical_timer_delay4();
	EL1_virtual_timer_delay1();
	EL1_virtual_timer_delay2();
	EL1_virtual_timer_delay3();
	EL1_virtual_timer_delay4();
	// Non_secure_EL2_virtual_timer_delay1();
	// Non_secure_EL2_virtual_timer_delay2();
	// Non_secure_EL2_virtual_timer_delay3();
	// Non_secure_EL2_virtual_timer_delay4();
	printf("Generic Timer Test Pass.\n\r");

	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif