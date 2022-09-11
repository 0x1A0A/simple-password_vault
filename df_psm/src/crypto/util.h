#ifndef __DFALSE_CRYPTO_UITL_H__
#define __DFALSE_CRYPTO_UITL_H__

#define ROT_L(a,n) ( a<<n ) | ( a>>(32-n) )
#define ROT_R(a,n) ( a>>n ) | ( a<<(32-n) )

#endif
