#include <platform_def.h>
#include <FreeRTOS_CLI.h>
#include <assert.h>
#include <stdio.h>


int load_and_run(bool use_cli)
{
	if(use_cli)
	{
		star_tinyCLI();
		assert(0);
	}
	else // use gpio
	{
		// TODO
	}
	return 0;
}