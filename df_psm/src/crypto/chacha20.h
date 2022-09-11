#ifndef __DF_CHACHA20_H__
#define __DF_CHACHA20_H__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t byte;
typedef uint32_t word;

struct chacha_block {
	word cc_const[4], key[8], counter, nonce[3];
};

struct chacha20  {
	word stream[16];
	struct chacha_block block;
	uint32_t index;
};

struct chacha20 *chacha20_create();
void chacha20_destroy(struct chacha20 *ctx);
void chacha20_block_init(struct chacha20 *ctx, byte key[32], byte nonce[12]);
void chacha20_key_stream_gen(struct chacha20 *ctx);

#endif
