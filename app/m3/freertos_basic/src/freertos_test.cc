#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// #include <iostream>
// using namespace std;

void task1(void* arg)
{
	(void)arg;
	volatile double i = 3.14;
	while(1)
	{
		printf("task1: %d\n\r", (int)i++); // newlib-nano dont support printing float.
		// cout << "task1: " << i++ << endl;
	}
}


void task2(void* arg)
{
	(void)arg;
	volatile double i = 3.1415926;
	while(1)
	{
		printf("task2: %d\n\r", (int)i++); // newlib-nano dont support printing float.
		// cout << "task1: " << i++ << endl;
	}
}

