#include <stdint.h>
#include <stdbool.h>

#define IMAGE_MAGIC 0x736B

typedef struct __attribute__((packed)) {
    uint16_t image_magic;
    uint8_t ecdsa_sig[64];
} image_hdr_t;


static inline bool image_hdr_valid(image_hdr_t *hdr)
{
	return hdr->image_magic == IMAGE_MAGIC;
}

bool image_verify(void* image, uint32_t image_size);