// #include "m3.h"
// #include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
// #include <iostream>
// #include <vector>
// #include <cstddef>
// using namespace std;
#include "complex.hh"

// namespace std
// {

// void __throw_out_of_range(char const*)
// {
//     while (true) {}
// }

// }

extern "C" {
/** Call constructors for static objects
 */
void call_init_array();

/** Call destructors for static objects
 */
void call_fini_array();

void task1(void* arg);
void task2(void* arg);
}

struct SomeClass
{
    virtual void someFunc();
};

void SomeClass::someFunc()
{
}


static complex c5(2, 1);
// extern "C"{ void * __dso_handle = 0 ;}
int main()
{
	
	call_init_array();

	// cout << "hello world form C++" << endl;
	// std::vector<int> v;
    // v.at(100) = 0;
    void complex_test();
    complex_test();

	static SomeClass someClass;
	someClass.someFunc();
	if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	call_fini_array();
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
	c3 = c5;
	// c1.real(100);
	// c1.imag(100);
	// cout << &c1 << " " << &c3 << endl;
	// cout << c1 << " " << c3 << endl;
}

