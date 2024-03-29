/*
 * FreeRTOS+CLI V1.0.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* FreeRTOS includes. */
// #include "FreeRTOS.h"
// #include "task.h"

/* Utils includes. */
#include "FreeRTOS_CLI.h"

/* If the application writer needs to place the buffer used by the CLI at a
fixed address then set configAPPLICATION_PROVIDES_cOutputBuffer to 1 in
FreeRTOSConfig.h, then declare an array with the following name and size in 
one of the application files:
	char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
*/
#ifndef configAPPLICATION_PROVIDES_cOutputBuffer
	#define configAPPLICATION_PROVIDES_cOutputBuffer 0
#endif

extern char __tinyCLI_CMD_START__[];
extern char __tinyCLI_CMD_END__[];
#define align_size __alignof__(CLI_Command_Definition_t)

/*
 * The callback function that is executed when "help" is entered.  This is the
 * only default command that is always present.
 */
static BaseType_t prvHelpCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Return the number of parameters that follow the command name.
 */
static int8_t prvGetNumberOfParameters( const char *pcCommandString );

/* The definition of the "help" command.  This command is always at the front
of the list of registered commands. */
static const CLI_Command_Definition_t xHelpCommand =
{
	"help",
	"Lists all the registered commands:\r\n",
	prvHelpCommand,
	0
};

/* The definition of the list of commands.  Commands that are registered are
added to this list. */
static CLI_Definition_List_Item_t xRegisteredCommands =
{
	&xHelpCommand,	/* The first command in the list is always the help command, defined in this file. */
	NULL			/* The next pointer is initialised to NULL, as there are no other registered commands yet. */
};

/* A buffer into which command outputs can be written is declared here, rather
than in the command console implementation, to allow multiple command consoles
to share the same buffer.  For example, an application may allow access to the
command interpreter by UART and by Ethernet.  Sharing a buffer is done purely
to save RAM.  Note, however, that the command console itself is not re-entrant,
so only one command interpreter interface can be used at any one time.  For that
reason, no attempt at providing mutual exclusion to the cOutputBuffer array is
attempted.

configAPPLICATION_PROVIDES_cOutputBuffer is provided to allow the application
writer to provide their own cOutputBuffer declaration in cases where the
buffer needs to be placed at a fixed address (rather than by the linker). */
#if( configAPPLICATION_PROVIDES_cOutputBuffer == 0 )
	static char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
#else
	extern char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
#endif

static CLI_Definition_List_Item_t *pxLastCommandInList = &xRegisteredCommands;

/*-----------------------------------------------------------*/
#if defined(configSUPPORT_MALLOC) && (configSUPPORT_MALLOC != 0)
	BaseType_t FreeRTOS_CLIRegisterCommand( const CLI_Command_Definition_t * const pxCommandToRegister )
	{
	CLI_Definition_List_Item_t *pxNewListItem;
	BaseType_t xReturn = pdFAIL;

		/* Check the parameter is not NULL. */
		configASSERT( pxCommandToRegister );

		/* Create a new list item that will reference the command being registered. */
		pxNewListItem = ( CLI_Definition_List_Item_t * ) pvPortMalloc( sizeof( CLI_Definition_List_Item_t ) );
		configASSERT( pxNewListItem );

		if( pxNewListItem != NULL )
		{
			taskENTER_CRITICAL();
			{
				/* Reference the command being registered from the newly created
				list item. */
				pxNewListItem->pxCommandLineDefinition = pxCommandToRegister;

				/* The new list item will get added to the end of the list, so
				pxNext has nowhere to point. */
				pxNewListItem->pxNext = NULL;

				/* Add the newly created list item to the end of the already existing
				list. */
				pxLastCommandInList->pxNext = pxNewListItem;

				/* Set the end of list marker to the new list item. */
				pxLastCommandInList = pxNewListItem;
			}
			taskEXIT_CRITICAL();

			xReturn = pdPASS;
		}

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
BaseType_t FreeRTOS_CLIRegisterCommandStatic( CLI_Definition_List_Item_t * const pxCommandItemToRegister )
{
BaseType_t xReturn = pdFAIL;

	/* Check the parameter is not NULL. */
	configASSERT( pxCommandItemToRegister );

	if( pxCommandItemToRegister != NULL )
	{
		taskENTER_CRITICAL();
		{
			/* The new list item will get added to the end of the list, so
			pxNext has nowhere to point. */
			pxCommandItemToRegister->pxNext = NULL;

			/* Add the newly created list item to the end of the already existing
			list. */
			pxLastCommandInList->pxNext = pxCommandItemToRegister;

			/* Set the end of list marker to the new list item. */
			pxLastCommandInList = pxCommandItemToRegister;
		}
		taskEXIT_CRITICAL();

		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CLIProcessCommand( const char * const pcCommandInput, char * pcWriteBuffer, size_t xWriteBufferLen  )
{
static const CLI_Definition_List_Item_t *pxCommand = NULL;
const CLI_Command_Definition_t *pxCommand_Class2 = (CLI_Command_Definition_t*)((uintptr_t)__tinyCLI_CMD_START__ + (align_size - (((uintptr_t)__tinyCLI_CMD_START__)&(align_size-1))));
BaseType_t xReturn = pdTRUE;
const char *pcRegisteredCommandString;
size_t xCommandStringLength;

	/* Note:  This function is not re-entrant.  It must not be called from more
	thank one task. */

	if( pxCommand == NULL )
	{
		// registered commands class 1.
		/* Search for the command string in the list of registered commands. */
		for( pxCommand = &xRegisteredCommands; pxCommand != NULL; pxCommand = pxCommand->pxNext )
		{
			pcRegisteredCommandString = pxCommand->pxCommandLineDefinition->pcCommand;
			xCommandStringLength = strlen( pcRegisteredCommandString );

			/* To ensure the string lengths match exactly, so as not to pick up
			a sub-string of a longer command, check the byte after the expected
			end of the string is either the end of the string or a space before
			a parameter. */
			if( strncmp( pcCommandInput, pcRegisteredCommandString, xCommandStringLength ) == 0 )
			{
				if( ( pcCommandInput[ xCommandStringLength ] == ' ' ) || ( pcCommandInput[ xCommandStringLength ] == 0x00 ) )
				{
					/* The command has been found.  Check it has the expected
					number of parameters.  If cExpectedNumberOfParameters is -1,
					then there could be a variable number of parameters and no
					check is made. */
					if( pxCommand->pxCommandLineDefinition->cExpectedNumberOfParameters >= 0 )
					{
						if( prvGetNumberOfParameters( pcCommandInput ) != pxCommand->pxCommandLineDefinition->cExpectedNumberOfParameters )
						{
							xReturn = pdFALSE;
						}
					}

					goto found;
				}
			}
		}

		// registered commands class 2.
		while(pxCommand_Class2 < (CLI_Command_Definition_t*)__tinyCLI_CMD_END__)
		{
			pcRegisteredCommandString = pxCommand_Class2->pcCommand;
			xCommandStringLength = strlen( pcRegisteredCommandString );

			if( strncmp( pcCommandInput, pcRegisteredCommandString, xCommandStringLength ) == 0 )
			{
				if( ( pcCommandInput[ xCommandStringLength ] == ' ' ) || ( pcCommandInput[ xCommandStringLength ] == 0x00 ) )
				{
					if( pxCommand_Class2->cExpectedNumberOfParameters >= 0 )
					{
						if( prvGetNumberOfParameters( pcCommandInput ) != pxCommand_Class2->cExpectedNumberOfParameters )
						{
							xReturn = pdFALSE;
						}
					}

					pxCommand = (CLI_Definition_List_Item_t*)pxCommand_Class2;
					goto found;
				}
			}

			pxCommand_Class2++;
		}
	}

found:
	if( ( pxCommand != NULL ) && ( xReturn == pdFALSE ) )
	{
		/* The command was found, but the number of parameters with the command
		was incorrect. */
		strncpy( pcWriteBuffer, "Incorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n", xWriteBufferLen );
		pxCommand = NULL;
	}
	else if( pxCommand != NULL )
	{
		/* Call the callback function that is registered to this command. */
		if((pxCommand >= (CLI_Definition_List_Item_t*)__tinyCLI_CMD_START__) && (pxCommand < (CLI_Definition_List_Item_t*)__tinyCLI_CMD_END__))
		{
			xReturn = ((CLI_Command_Definition_t*)pxCommand)->pxCommandInterpreter( pcWriteBuffer, xWriteBufferLen, pcCommandInput );
		}
		else
		{
			xReturn = pxCommand->pxCommandLineDefinition->pxCommandInterpreter( pcWriteBuffer, xWriteBufferLen, pcCommandInput );
		}

		/* If xReturn is pdFALSE, then no further strings will be returned
		after this one, and	pxCommand can be reset to NULL ready to search
		for the next entered command. */
		if( xReturn == pdFALSE )
		{
			pxCommand = NULL;
		}
	}
	else
	{
		/* pxCommand was NULL, the command was not found. */
		strncpy( pcWriteBuffer, "Command not recognised.  Enter 'help' to view a list of available commands.\r\n", xWriteBufferLen );
		xReturn = pdFALSE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

char *FreeRTOS_CLIGetOutputBuffer( void )
{
	return cOutputBuffer;
}
/*-----------------------------------------------------------*/

const char *FreeRTOS_CLIGetParameter( const char *pcCommandString, UBaseType_t uxWantedParameter, BaseType_t *pxParameterStringLength )
{
UBaseType_t uxParametersFound = 0;
const char *pcReturn = NULL;

	*pxParameterStringLength = 0;

	while( uxParametersFound < uxWantedParameter )
	{
		/* Index the character pointer past the current word.  If this is the start
		of the command string then the first word is the command itself. */
		while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) != ' ' ) )
		{
			pcCommandString++;
		}

		/* Find the start of the next string. */
		while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) == ' ' ) )
		{
			pcCommandString++;
		}

		/* Was a string found? */
		if( *pcCommandString != 0x00 )
		{
			/* Is this the start of the required parameter? */
			uxParametersFound++;

			if( uxParametersFound == uxWantedParameter )
			{
				/* How long is the parameter? */
				pcReturn = pcCommandString;
				while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) != ' ' ) )
				{
					( *pxParameterStringLength )++;
					pcCommandString++;
				}

				if( *pxParameterStringLength == 0 )
				{
					pcReturn = NULL;
				}

				break;
			}
		}
		else
		{
			break;
		}
	}

	return pcReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvHelpCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
static const CLI_Definition_List_Item_t * pxCommand = (CLI_Definition_List_Item_t*)(uintptr_t)0xffffffff;
BaseType_t xReturn;

	( void ) pcCommandString;

	if( pxCommand == (CLI_Definition_List_Item_t*)(uintptr_t)0xffffffff )
	{
		/* Reset the pxCommand pointer back to the start of the list. */
		pxCommand = &xRegisteredCommands;
	}

	if( pxCommand == NULL )
	{
		if((uintptr_t)__tinyCLI_CMD_START__ == (uintptr_t)__tinyCLI_CMD_END__) // Class2 maybe empty
			pxCommand = (CLI_Definition_List_Item_t*)(uintptr_t)__tinyCLI_CMD_END__;
		else
			pxCommand = (CLI_Definition_List_Item_t*)((uintptr_t)__tinyCLI_CMD_START__ + (align_size - (((uintptr_t)__tinyCLI_CMD_START__)&(align_size-1))));
	}

	/* Return the next command help string, before moving the pointer on to
	the next command in the list. */
	if( pxCommand == (CLI_Definition_List_Item_t*)__tinyCLI_CMD_END__ )
	{
		/* There are no more commands in the list, so there will be no more
		strings to return after this one and pdFALSE should be returned. */
		*pcWriteBuffer = '\0';
		pxCommand = (CLI_Definition_List_Item_t*)(uintptr_t)0xffffffff;
		xReturn = pdFALSE;
	}
	else if((pxCommand >= (CLI_Definition_List_Item_t*)__tinyCLI_CMD_START__) && (pxCommand < (CLI_Definition_List_Item_t*)__tinyCLI_CMD_END__))
	{
		strncpy( pcWriteBuffer, ((CLI_Command_Definition_t*)pxCommand)->pcHelpString, xWriteBufferLen );
		pxCommand = (CLI_Definition_List_Item_t*)(uintptr_t)((uintptr_t)pxCommand + sizeof(CLI_Command_Definition_t));
		xReturn = pdTRUE;
	}
	else
	{
		strncpy( pcWriteBuffer, pxCommand->pxCommandLineDefinition->pcHelpString, xWriteBufferLen );
		pxCommand = pxCommand->pxNext;
		xReturn = pdTRUE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static int8_t prvGetNumberOfParameters( const char *pcCommandString )
{
int8_t cParameters = 0;
BaseType_t xLastCharacterWasSpace = pdFALSE;

	/* Count the number of space delimited words in pcCommandString. */
	while( *pcCommandString != 0x00 )
	{
		if( ( *pcCommandString ) == ' ' )
		{
			if( xLastCharacterWasSpace != pdTRUE )
			{
				cParameters++;
				xLastCharacterWasSpace = pdTRUE;
			}
		}
		else
		{
			xLastCharacterWasSpace = pdFALSE;
		}

		pcCommandString++;
	}

	/* If the command string ended with spaces, then there will have been too
	many parameters counted. */
	if( xLastCharacterWasSpace == pdTRUE )
	{
		cParameters--;
	}

	/* The value returned is one less than the number of space delimited words,
	as the first word should be the command itself. */
	return cParameters;
}



/********/
#include <stdio.h>

#pragma weak star_tinyCLI

#define MAX_INPUT_LENGTH 50
#define MAX_OUTPUT_LENGTH 96

static const char *const pcWelcomeMessage = "Command server.Type Help to view a list of registered commands.";
static char clrline[MAX_INPUT_LENGTH + 1];
void star_tinyCLI(void)
{
	// Peripheral_Descriptor_t xConsole;
	int8_t cRxedChar, cInputIndex = 0;
	BaseType_t xMoreDataToFollow;
	/* The input and output buffers are declared static to keep them off the stack. */
	static char pcOutputString[MAX_OUTPUT_LENGTH], pcInputString[MAX_INPUT_LENGTH];

	/* This code assumes the peripheral being used as the console has already
    been opened and configured, and is passed into the task as the task
    parameter.  Cast the task parameter to the correct type. */
	// xConsole = ( Peripheral_Descriptor_t ) pvParameters;

	/* Send a welcome message to the user knows they are connected. */
	// FreeRTOS_write( xConsole, pcWelcomeMessage, strlen( pcWelcomeMessage ) );
	printf("%s\n\r", pcWelcomeMessage);

	memset(clrline, ' ', MAX_INPUT_LENGTH);
	clrline[MAX_INPUT_LENGTH] = 0;

	putchar('>');
	for (;;) 
	{
		cRxedChar = getchar();

		if (cRxedChar == '\r') 
		{
			printf("\n\r");

			if (cInputIndex == 0) 
			{
				putchar('>');
				continue;
			}

			do 
			{
				xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
					pcInputString, /* The command string.*/
					pcOutputString, /* The output buffer. */
					MAX_OUTPUT_LENGTH /* The size of the output buffer. */
				);
				printf("%s", pcOutputString);
			} while (xMoreDataToFollow != pdFALSE);

			cInputIndex = 0;
			memset(pcInputString, 0x00, MAX_INPUT_LENGTH);
			putchar('>');
		}
		else 
		{
			if ((cRxedChar>=32) && (cRxedChar<=126)) // 可显示字符
			{
				if (cInputIndex < MAX_INPUT_LENGTH) 
				{
					pcInputString[cInputIndex] = cRxedChar;
					putchar(cRxedChar);
					cInputIndex++;
				}
			}
			else if ((cRxedChar == '\b') || (cRxedChar == 127)) // 退格&删除
			{
				if (cInputIndex > 0) 
				{
					cInputIndex--;
					pcInputString[cInputIndex] = 0;
					putchar('\b');
					putchar(' ');
					putchar('\b');
				}
			}
			else //不支持的字符
			{

			}
	}
	}
}


