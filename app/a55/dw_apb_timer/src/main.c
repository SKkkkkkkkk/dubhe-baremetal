#ifndef QEMU
#include <stdio.h>
#include <stdbool.h>

int main()
{
	void dw_apb_timer_test(bool sample);
	dw_apb_timer_test(true);
	return 0;
}

#else
#include "main_qemu.c"
#endif