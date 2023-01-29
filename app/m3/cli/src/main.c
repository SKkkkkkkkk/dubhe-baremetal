#include "m3.h"
#include <stdio.h>

int main()
{
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	printf("hello world.\n\r");
	
	void CLIRegisterCommand(void);
	void vCommandConsoleTask(void *pvParameters);
	CLIRegisterCommand();
	vCommandConsoleTask(NULL);
	return 0;
}