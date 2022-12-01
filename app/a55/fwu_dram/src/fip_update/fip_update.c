#include "fip_update.h"
#include "uart_update.h"

int fip_update(enum input_device in, enum output_device out)
{
	int ret = -1;
	switch (in)
	{
	case UART:
		ret = uart_update(out);
		break;
	case USB:
		printf("USB: Todo...\n\r");
		//ret = usb_update();
		break;
	default:
		break;
	}
	return ret;
}