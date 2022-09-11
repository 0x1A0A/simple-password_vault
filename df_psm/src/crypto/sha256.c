#include "sha256.h"
#include "util.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const word SHA256_INIT_STATE[] = {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19,
};

static const word SHA256_ROUND_CONST[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_init(struct df_sha256context *ctx)
{
	memset( ctx->data_chunk, 0, 64);
	ctx->total_bit = ctx->current_index = 0;
	for (size_t i=0; i<8; i++) ctx->state[i] = SHA256_INIT_STATE[i]; 
}

static void sha256_process(struct df_sha256context *ctx)
{
	word w[64], h[8], s[2], ch, temp[2], maj;
	
	// change data to 32 bit big-endian
	for ( size_t i=0, j=0; i<16; ++i, j+=4 ) {
		w[i] = 	ctx->data_chunk[j] 		<< 24 | 
				ctx->data_chunk[j+1] 	<< 16 | 
				ctx->data_chunk[j+2] 	<< 8 | 
				ctx->data_chunk[j+3];
	}

	for ( size_t i=16; i<64; ++i) {
		s[0] = (ROT_R(w[i-15],7)) ^ (ROT_R(w[i-15], 18)) ^ (w[i-15] >> 3);
		s[1] = (ROT_R(w[i-2],17)) ^ (ROT_R(w[i-2],19)) ^ (w[i-2] >> 10);
		w[i] = w[i-16] + s[0] + w[i-7] + s[1];
	}
	
	for ( size_t i=0; i<8; ++i)
		h[i] = ctx->state[i];
	
	// compression
	for ( size_t i=0; i<64; ++i ) {
		s[1] = (ROT_R(h[4],6)) ^ (ROT_R(h[4], 11)) ^ (ROT_R(h[4], 25));
		ch = (h[4] & h[5]) ^ ((~h[4]) & h[6]);
		temp[0] = h[7] + s[1] + ch + SHA256_ROUND_CONST[i] + w[i];
		s[0] = (ROT_R(h[0],2)) ^ (ROT_R(h[0],13)) ^ (ROT_R(h[0], 22));
		maj = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
		temp[1] = s[0] + maj;

		h[7] = h[6];
		h[6] = h[5];
		h[5] = h[4];
		h[4] = h[3] + temp[0];
		h[3] = h[2];
		h[2] = h[1];
		h[1] = h[0];
		h[0] = temp[0] + temp[1];
	}

	for (size_t i=0; i<8; ++i)
		ctx->state[i] += h[i];
}

struct df_sha256context* sha256context_create()
{
	struct df_sha256context *o = (struct df_sha256context*) malloc( sizeof(struct df_sha256context) );
	
	sha256_init(o);

	return o;
}

uint8_t sha256context_add(struct df_sha256context *ctx, byte *in, size_t length )
{
	if ( in == NULL ) return 0;

	byte *data = (byte *)in;
	// create 512-bit chunk
	size_t i=0;
	while (i < length) {
		ctx->data_chunk[ctx->current_index++] = data[i++];
		++ctx->total_bit;
		if (ctx->current_index == 64) { // need new chunk
			// process this chunk first because it's full
			sha256_process(ctx);
			ctx->current_index = 0;
		}
	}
}

uint8_t sha256context_end(struct df_sha256context *ctx, byte out[32])
{
	// process the last chunk
	uint16_t i = ctx->current_index;
	if ( i < 56 ) {
		ctx->data_chunk[i++] = 0x80;
		while ( i < 56 ) ctx->data_chunk[i++] = 0x00;
	} else {
		ctx->data_chunk[i++] = 0x80;
		while (i<64) 
			ctx->data_chunk[i++] = 0x00;
		// process again
		sha256_process(ctx);
		memset(ctx->data_chunk, 0, 56);
	}
	
	ctx->total_bit <<= 3; // total bit of input data

	for (i=0; i<8; ++i) ctx->data_chunk[63-i] = ((uint8_t*)&(ctx->total_bit))[i];

	// do one last process
	sha256_process(ctx);

	// digest result to output (little-endian)
	for ( i=0; i<8; ++i )
		for ( size_t j=0; j<4; ++j )
			out[j+i*4] = ((byte *)&(ctx->state[i]))[3-j];

	return 1;
}

void sha256context_destroy(struct df_sha256context *ctx)
{
	free(ctx);
	ctx = NULL;
}
