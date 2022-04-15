#include "ri5cy.h"

int main()
{
	system_init();
	while(1)
	{
		asm volatile("nop");
	}
}