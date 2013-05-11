/*
  Based on
    SHA-1 in C
    By Steve Reid <steve@edmweb.com>
    100% Public Domain
*/
#ifndef LIBUTILS_SHA1
#define LIBUTILS_SHA1

#include <stdint.h>

typedef struct
{
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHA1Context;

#define SHA1_SIZE_DIGEST 20
#define SHA1_SIZE_HEX 40

void sha1_init(SHA1Context *context);
void sha1_update(SHA1Context *context, const uint8_t *data, uint32_t len);
void sha1_final(SHA1Context *context, uint8_t digest[]);
void sha1_hex(const uint8_t digest[SHA1_SIZE_DIGEST], char hex_out[SHA1_SIZE_HEX]);

#endif
