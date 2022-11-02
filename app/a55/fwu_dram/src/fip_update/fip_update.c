#include <fip_update.h>
#include <ymodem_update.h>

int normal_fwu_update_fip()
{
	int ret;
	enum input_device i_device = UART;
	enum output_device o_device = NOR_FLASH;
	switch (i_device)
	{
	case UART:
		ret = ymodem_update(o_device);
		break;
	case USB:
		//ret = usb_update();
		printf("USB: Todo...\n\r");
		assert(0);
		break;
	default:
		break;
	}
	return ret;
}