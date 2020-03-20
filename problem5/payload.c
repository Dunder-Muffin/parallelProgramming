#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "payload.h"
#include "sha256.h"


struct payload_t *payload_new()
{
	struct payload_t *p = malloc(sizeof(struct payload_t));
	if (!p) {
		return NULL;
	}
	p->data = calloc(1, DATA_SIZE);
	if (!p->data) {
		return NULL;
	}
	return p;
}

struct payload_t *payload_gen(const char *repo)
{
	struct payload_t *p = payload_new();
	if (!p) {
		return NULL;
	}

	int len = strlen(repo);
	int limit = len;
	for (uint offset = sizeof(int64_t); offset < DATA_SIZE; offset += limit) {
		if (offset + len > DATA_SIZE) {
			limit = DATA_SIZE - offset;
		}
		strncpy(p->data + offset, repo, limit);
	}
	return p;
}

void payload_checksum(const struct payload_t *p, unsigned char sum[])
{
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, (void *)p->data, DATA_SIZE);
	sha256_final(&ctx, sum);

	// 1/16 probability to sleep for [0..255] microsecnods.
	if (sum[0] < 16) {
		usleep(sum[1]);
	}
}

void payload_set_magic(struct payload_t *p, int64_t magic)
{
	if (magic > MAX_MAGIC) {
		magic = MAX_MAGIC;
	}
	*((int64_t *)p->data) = magic;
}

void payload_free(struct payload_t *p)
{
	free(p->data);
	free(p);
}

