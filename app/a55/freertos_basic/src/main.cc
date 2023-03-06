#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
// #ifdef __cplusplus
    extern "C" {
// #endif
#include "irq_ctrl.h"
#ifndef QEMU
	#include "system_counter.h"
#endif
	}
#include "complex.hh"

// #include <iostream> // cant use
// using namespace std;


extern "C"{
	void task1(void* arg);
	void task2(void* arg);
}
extern "C" void config_mmu();
int main()
{	
	printf("hello world.\n\r");
	
	config_mmu();
	printf("hello world after mmu enabled.\n\r");
	void complex_test();
	complex_test();

	// cout << "hello from C++" << endl; // cant use
#ifndef QEMU
	initSystemCounter(0, 0);
#endif
	IRQ_Initialize();

	if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	return 0;
}



void complex_test()
{
	// 3种初始化方式
	static complex c1(2, 1);
	complex c2(1);
	complex c3 = c1;
	complex c4(c1);

	// 操作符重载
	// cout << c1 << endl;
	// cout << c2 << endl;

	// 函数重载
	
	// cout << c1 << endl;
	// c1.real(c1.real()+1);
	// c1.imag(c1.imag()+1);
	// cout << c1 << endl;

	// c1 += c2;
	// cout << c1 << endl;

	// cout << c1 << " " << c2 << " " << c3 << " " << c4 << endl;
    // printf()

	c3 = c1;
	// c3 = c5;
	// c1.real(100);
	// c1.imag(100);
	// cout << &c1 << " " << &c3 << endl;
	// cout << c1 << " " << c3 << endl;
}

