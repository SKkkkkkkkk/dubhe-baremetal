#ifndef QEMU
#include <stdio.h>

int main()
{
	printf("2022.09.14\n\r");
	printf("hello world.\n\r");
	return 0;
}

#else
#include "main_qemu.c"
#endif