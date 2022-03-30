#include "main.h"
// #include <stdio.h>
#include "wakeup_core.h"
#include "pl001.h"


void core1_c_entry(void)
{
	uart_write("core1_c_entry\n");
	void core2_c_entry(void);
	wakeup_core(2, core2_c_entry);
	while(1);
}

void core2_c_entry(void)
{
	uart_write("core2_c_entry\n");
	void core3_c_entry(void);
	wakeup_core(3, core3_c_entry);
	while(1);
}

void core3_c_entry(void)
{
	uart_write("core3_c_entry\n");
	while(1);
}

int main()
{
	// printf("hello world!\n");
	uart_write("hello world!\n");
	wakeup_core(1, core1_c_entry);
	// wakeup_core(2, core2_c_entry);
	// wakeup_core(3, core3_c_entry);
	while(1);
	return 0;
}
