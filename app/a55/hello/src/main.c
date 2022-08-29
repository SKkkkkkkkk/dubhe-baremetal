#ifndef QEMU
#include <stdio.h>

int main()
{
	printf("hello world.\n\r");
	return 0;
}

#else
#include "main_qemu.c"
#endif