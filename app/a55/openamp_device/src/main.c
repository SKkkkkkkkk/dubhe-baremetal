#include <stdio.h>
#include "openamp/open_amp.h"
#include "metal/version.h"
#include "openamp/version.h"

#define LPRINTF printf

int main()
{
	LPRINTF("openamp lib version: %s (", openamp_version());
	LPRINTF("Major: %d, ", openamp_version_major());
	LPRINTF("Minor: %d, ", openamp_version_minor());
	LPRINTF("Patch: %d)\r\n", openamp_version_patch());

	LPRINTF("libmetal lib version: %s (", metal_ver());
	LPRINTF("Major: %d, ", metal_ver_major());
	LPRINTF("Minor: %d, ", metal_ver_minor());
	LPRINTF("Patch: %d)\r\n", metal_ver_patch());

	int openamp_demo(int role);
	openamp_demo(VIRTIO_DEV_DEVICE);

	return 0;
}