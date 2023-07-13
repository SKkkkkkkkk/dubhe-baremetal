#include <stdio.h>
#include <uECC.h>
#include <sha2.h>

static const uint8_t PUBKEY[] = {
    0xed,0x8c,0xc6,0xd3,0x12,0x25,0x9b,0x80,0x09,0x1a,0x8b,0xdd,0xb1,0x15,
    0x84,0xf4,0xd7,0x96,0x49,0xa8,0x43,0x75,0x20,0x18,0xcc,0x24,0xcb,0x0f,0xe3,
    0x49,0x1b,0x7d,0xfa,0xe8,0x82,0x7f,0x43,0x5a,0x3d,0x0e,0x2b,0x44,0x8a,0xa3,
    0xf1,0x51,0x19,0x74,0xfe,0x17,0xcb,0x07,0x02,0xe7,0xba,0x61,0xf0,0xe8,0xa1,
    0xc0,0x68,0xf1,0xb3,0x52
  // 64 bytes
};

static void prv_sha256(const void *buf, uint32_t size, uint8_t *hash_out)
{
  cf_sha256_context ctx;
  cf_sha256_init(&ctx);
  cf_sha256_update(&ctx, buf, size);
  cf_sha256_digest_final(&ctx, hash_out);
}

int main()
{
	printf("M2v5: "BUILD_TIMESTAMP"\n\r");
	printf("git hash: "GIT_HASH"\n\r");
	printf("A55: hello world.\n\r");

	// 1. check public key.
	const struct uECC_Curve_t *curve = uECC_secp256k1();
	if (!uECC_valid_public_key(PUBKEY, curve)) {
		printf("Public key is NOT valid\n");
	} else {
		printf("Public key is valid\n");
	}

	// 2. generate sha256 hash.
	uint8_t hash_sha256[CF_SHA256_HASHSZ];
    prv_sha256((void*)0xb0000000, 192515, hash_sha256);

	// 3. verify signature.
	if (!uECC_verify(PUBKEY, hash_sha256, CF_SHA256_HASHSZ, (void*)0xf0000000/*signature*/, curve))
	{
		printf("Signature is NOT valid\n");
	}
	else
	{
		printf("Signature is valid\n");
	}

	return 0;
}