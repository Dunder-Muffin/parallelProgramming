#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdlib.h>

#define DATA_SIZE 2048
#define MAX_MAGIC 1048576

struct payload_t {
	char *data;
};

struct payload_t *payload_gen(const char *repo);

void payload_checksum(const struct payload_t *p, unsigned char sum[]);
void payload_set_magic(struct payload_t *p, int64_t magic);
void payload_free(struct payload_t *p);

#endif
