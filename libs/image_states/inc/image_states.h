#ifndef __IMAGE_STATES_H__
#define __IMAGE_STATES_H__

#include <stdint.h>

typedef union image_state {
	uint32_t image_state_u32;
	struct {	
		uint32_t state: 28; /*!< bit:  0..27  */
		uint32_t image: 4;  /*!< bit:  28..31 */
	} image_state;
} image_state_t;


typedef enum {
	BL1 = 0,
	BL2,
	BL31,
	NS_BL1U,
	A55_IMAGE_MAX
} a55_image_name;


typedef enum {
	M3_BL1 = 0,
	M3_BL2,
	M3_IMAGE_MAX
} m3_image_name;


/**
 * @brief set a55 image state.
 * @param core_id choose core, 0, 1, 2, 3.
 * @param image_state the image_state to be set.
 */
void a55_set_image_state(uint8_t core_id, image_state_t image_state);

/**
 * @brief get a55 image state.
 * @param core_id choose a55 core, 0, 1, 2, 3.
 * @return return the image_state.
 */
image_state_t a55_get_image_state(uint8_t core_id);


/**
 * @brief set m3 image state.
 * @param image_state the image_state to be set.
 */
void m3_set_image_state(image_state_t image_state);

/**
 * @brief set m3 image state.
 * @return return the image_state.
 */
image_state_t m3_get_image_state();

#endif