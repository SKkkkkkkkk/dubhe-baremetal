#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "irq_ctrl.h"
#ifndef QEMU
	#include "system_counter.h"
#endif
#include "complex.hh"
#include "string.hh"
#include <vector>

// extern "C"{
	void task1(void* arg);
	void task2(void* arg);
// }

int main()
{
	printf("hello world.\n\r");
	void complex_test();
	complex_test();
	void string_test();

#ifndef QEMU
	initSystemCounter(0, 0);
#endif
	IRQ_Initialize();

	if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE*10, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE*10, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	return 0;
}


static complex c5(1,2);
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
	c1.real(c1.real()+1);
	c1.imag(c1.imag()+1);
	// cout << c1 << endl;

	c1 += c2;
	// cout << c1 << endl;

	// cout << c1 << " " << c2 << " " << c3 << " " << c4 << endl;
    // printf()

	c3 = c1;
	c3 = c5;
	c1.real(100);
	c1.imag(100);
	// cout << &c1 << " " << &c3 << endl;
	// cout << c1 << " " << c3 << endl;
}



void string_test()
{
	printf("%ld\n\r", String1::get_instance_num());
	char arr[] = "hello world";
	String1 str(arr);
	String1 str1 = str;
	String1 str2(str1);
	String1 str3;
	str3 = str2;
	// static String1 str4;


	((char*)(uintptr_t)str.get_str())[4] = '?';
	printf("0x%lx 0x%lx 0x%lx 0x%lx\n\r", (uintptr_t)str.get_str(), (uintptr_t)str1.get_str(), (uintptr_t)str2.get_str(), (uintptr_t)str3.get_str());
	// cout << str.get_str() << "\n\r" << str1.get_str() << "\n\r" << str2.get_str() << "\n\r" << str3.get_str() << "\n\r" << str4.get_str() << endl;
	// cout << str.get_instance_num() << "\n\r" << str1.get_instance_num() << "\n\r" << str2.get_instance_num() << "\n\r" << str3.get_instance_num() << "\n\r" << str4.get_instance_num() << endl;
}

