#include <stdint.h>
#include <stdio.h>
volatile uint8_t arry[8] = {1};
volatile uint8_t arry_bss[8];
volatile const uint8_t const_arry = 0xa5;
volatile const uint8_t const_arry_bss = 0x5a;

void func()
{
	arry[0] = const_arry;
	arry_bss[0] = const_arry_bss;
}
int main()
{
	func();
	printf("hello world.\n");
	while(1);
}