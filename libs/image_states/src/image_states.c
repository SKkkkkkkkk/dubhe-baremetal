#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "image_states.h"
#include "chip_mem_layout.h"

#define CORE0_IMAGE_STATE_REG_OFFSITE 0xEF0
#define CORE1_IMAGE_STATE_REG_OFFSITE 0xEF4
#define CORE2_IMAGE_STATE_REG_OFFSITE 0xEF8
#define CORE3_IMAGE_STATE_REG_OFFSITE 0xEFC

#define CM3_IMAGE_STATE_REG_OFFSITE   0xFF0

void a55_set_image_state(uint8_t core_id, image_state_t image_state)
{
	if(core_id>3)	return;

	REG32(SYSCTRL_BASE + (CORE0_IMAGE_STATE_REG_OFFSITE + core_id*4)) = image_state.image_state_u32;
	return;
}


image_state_t a55_get_image_state(uint8_t core_id)
{
	image_state_t image_state;
	if(core_id>3)
	{
		image_state.image_state_u32 = 0xFFFFFFFFU;
		return image_state;
	}
	
	image_state.image_state_u32 = REG32(SYSCTRL_BASE + (CORE0_IMAGE_STATE_REG_OFFSITE + core_id*4));
	return image_state;
}


void m3_set_image_state(image_state_t image_state)
{
	REG32(SYSCTRL_BASE + CM3_IMAGE_STATE_REG_OFFSITE) = image_state.image_state_u32;
	return;
}


image_state_t m3_get_image_state()
{
	return (image_state_t)REG32(SYSCTRL_BASE + CM3_IMAGE_STATE_REG_OFFSITE);
}
