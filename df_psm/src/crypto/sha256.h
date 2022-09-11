#ifndef __DF_SHA256_H__
#define __DF_SHA256_H__

#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte;
typedef uint32_t word;

struct df_sha256context {
	byte data_chunk[64];
	word state[8];
	uint64_t total_bit;
	uint16_t current_index; // current index of 
};

// manual
//		create sha256contex
struct df_sha256context* sha256context_create();
//		add input to sha256contex
uint8_t sha256context_add(struct df_sha256context *ctx, byte *in, size_t length);
//		produce final output
uint8_t sha256context_end(struct df_sha256context *ctx, byte out[32]);
// 		destroy sha256 contex
void sha256context_destroy(struct df_sha256context *ctx);

#endif