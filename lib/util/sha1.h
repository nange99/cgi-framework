/*
 * SHA1 routine optimized to do word accesses rather than byte accesses,
 * and to avoid unnecessary copies into the context array.
 *
 * This was initially based on the Mozilla SHA1 implementation, although
 * none of the original Mozilla code remains.
 */

#ifndef SHA1_H_
#define SHA1_H_

typedef struct {
	unsigned long long size;
	unsigned int H[5];
	unsigned int W[16];
} blk_SHA_CTX;

void blk_SHA1_Init(blk_SHA_CTX *ctx);
void blk_SHA1_Update(blk_SHA_CTX *ctx, const void *dataIn, unsigned long len);
void blk_SHA1_Final(unsigned char hashout[20], blk_SHA_CTX *ctx);
char *sha1_to_hex(const unsigned char *sha1);

#define sha_ctx		blk_SHA_CTX
#define sha1_init	blk_SHA1_Init
#define sha1_update	blk_SHA1_Update
#define sha1_final	blk_SHA1_Final

#endif /* SHA1_H_ */
