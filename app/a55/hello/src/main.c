#ifndef QEMU
#include <stdio.h>

int main()
{
	printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	printf("A55: hello world.\n\r");
	return 0;
}

#else
#include "main_qemu.c"
#endif