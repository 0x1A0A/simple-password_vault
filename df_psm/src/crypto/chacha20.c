#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "chacha20.h"

#define QR(a, b, c, d) (			\
	a += b,  d ^= a,  d = ROT_L(d,16),	\
	c += d,  b ^= c,  b = ROT_L(b,12),	\
	a += b,  d ^= a,  d = ROT_L(d, 8),	\
	c += d,  b ^= c,  b = ROT_L(b, 7))
#define ROUNDS 20

struct chacha20 *chacha20_create()
{
	struct chacha20 *neww = (struct chacha20*)malloc(sizeof(struct chacha20));
	neww->index = 0;
	return neww;
}

void chacha20_destroy(struct chacha20 *ctx)
{
	free(ctx);
}

void chacha20_block_init(struct chacha20 *ctx, byte key[32], byte nonce[12])
{
	strncpy( (char*)(ctx->block.cc_const), "expand 32-byte k", 16 );
	strncpy( (char*)(ctx->block.key), (char*)key, 32 );
	ctx->block.counter = 0;
	strncpy( (char*)(ctx->block.nonce), (char*)nonce,12 );
}

void chacha20_key_stream_gen(struct chacha20 *ctx)
{
	memcpy(ctx->stream, &(ctx->block), 64);
	word *cell = ctx->stream;
	for (size_t i=0; i<ROUNDS; i+=2) {
		QR(cell[0], cell[4], cell[ 8], cell[12]); // column 0
		QR(cell[1], cell[5], cell[ 9], cell[13]); // column 1
		QR(cell[2], cell[6], cell[10], cell[14]); // column 2
		QR(cell[3], cell[7], cell[11], cell[15]); // column 3
		// Even round
		QR(cell[0], cell[5], cell[10], cell[15]); // diagonal 1
		QR(cell[1], cell[6], cell[11], cell[12]); // diagonal 2
		QR(cell[2], cell[7], cell[ 8], cell[13]); // diagonal 3
		QR(cell[3], cell[4], cell[ 9], cell[14]); // diagonal 4
	}

	// mix it with original block (ADD)
	for (size_t i=0; i<16; ++i) {
		cell[i] += ((word*)&(ctx->block))[i];
	}
}
