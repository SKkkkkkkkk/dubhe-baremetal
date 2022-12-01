#ifndef __FIP_UPDATE_H__
#define __FIP_UPDATE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <uuid.h>
#include <uuid_utils.h>
#include <fip/firmware_image_package.h>

enum input_device {
	UART = 0,
	USB = 1
};

enum output_device {
	NOR_FLASH = 0,
	EMMC = 1
};

static inline bool is_valid_header(fip_toc_header_t *header)
{
	if ((header->name == TOC_HEADER_NAME) && (header->serial_number != 0)) {
		return true;
	} else {
		return false;
	}
}


int fip_update(enum input_device in, enum output_device out);

#endif