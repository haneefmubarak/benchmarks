#pragma once

#include <assert.h>
#include <stdint.h>

typedef struct {
	uint8_t	k[32];
	uint8_t	iv[32];
	uint8_t	d[1024];
	uint8_t	pt[1024 * 1024];
} input;

#define	BENCHMARK_BUFFER	(sizeof (input))

#include "../../benchmark.h"

#include <sodium.h>

// ciphertext + auth tag
const int ciphertext_size = sizeof (((input){ 0 }).pt) + 32;

BENCHMARK_SETUP (b) {
	assert (!sodium_init ());

	randombytes_buf (b, BENCHMARK_BUFFER);
	return malloc (ciphertext_size);
}
