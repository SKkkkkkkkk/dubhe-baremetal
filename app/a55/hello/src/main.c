#ifndef QEMU
#include <stdio.h>

int main()
{
	printf("git hash: "GIT_HASH"\n\r");
	printf("hello world.\n\r");
	return 0;
}

#else
#include "main_qemu.c"
#endif