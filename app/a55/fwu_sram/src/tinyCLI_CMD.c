#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "FreeRTOS_CLI.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif



/****COMMAND1****/
static BaseType_t print_helloworld(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pcParameter1;
	BaseType_t xParameter1StringLength;

    /* Obtain the name of the source file, and the length of its name, from
    the command string. The name of the source file is the first parameter. */
    pcParameter1 = FreeRTOS_CLIGetParameter
                        (
                          /* The command string itself. */
                          pcCommandString,
                          /* Return the first parameter. */
                          1,
                          /* Store the parameter string length. */
                          &xParameter1StringLength
                        );
	
	/* Terminate both file names. */
    // pcParameter1[ xParameter1StringLength ] = 0x00;

	strncpy(pcWriteBuffer, "hello ", 6);
	strncpy(pcWriteBuffer+6, pcParameter1, xParameter1StringLength);
	strncpy((char*)((uintptr_t)pcWriteBuffer+6+xParameter1StringLength), "\n\r", 3);

	return pdFALSE;
}

tinyCLI_CMD(hello) = { .pcCommand = "hello",
		.pcHelpString = "hello [arg1]\r\n",
		.pxCommandInterpreter = print_helloworld,
		.cExpectedNumberOfParameters = 1 };


/****COMMAND2****/
// int load_by_uart(uintptr_t sram_addr);

// tinyCLI_CMD(setpc) = { .pcCommand = "setpc",
// 		.pcHelpString = "setpc [addr]\r\n",
// 		.pxCommandInterpreter = print_helloworld,
// 		.cExpectedNumberOfParameters = 1 };



/****COMMAND3****/
static BaseType_t CMD_uart_load(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	// const char *pcParameter1;
	// BaseType_t xParameter1StringLength;

    // pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
	int uart_load(/*uintptr_t sram_addr*/);
	uart_load();

	return false;
}

tinyCLI_CMD(load_by_uart) = { .pcCommand = "uart_load",
		.pcHelpString = "uart_load\r\n",
		.pxCommandInterpreter = CMD_uart_load,
		.cExpectedNumberOfParameters = 0 };



/****COMMAND4****/
tinyCLI_CMD(load_by_usb) = { .pcCommand = "load_by_usb",
		.pcHelpString = "load_by_usb\r\n",
		.pxCommandInterpreter = print_helloworld,
		.cExpectedNumberOfParameters = 0 };



#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif