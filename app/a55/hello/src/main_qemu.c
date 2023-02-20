#include <stdio.h>
#include "wakeup_core.h"

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

void secondary_entry(void)
{
	printf("this is core.%d\n\r", get_core_id());
	while(1);
}

int main()
{
	printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	printf("A55: hello world.\n\r");
	wakeup_core(1, secondary_entry);
	wakeup_core(2, secondary_entry);
	wakeup_core(3, secondary_entry);
	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif