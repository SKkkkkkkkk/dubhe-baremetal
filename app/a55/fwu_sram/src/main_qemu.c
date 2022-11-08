#include <stdio.h>

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

int main()
{
	printf("git hash: "GIT_HASH"\n\r");
	printf("FWU SRAM:\n\r");
	printf("\tddr init.\n\r");
	printf("\tgo to FWU DDR.\n\r");
	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif