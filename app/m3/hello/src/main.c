#include "m3.h"
#include <stdio.h>

int main()
{
	printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	printf("M3: hello world.\n\r");
	return 0;
}