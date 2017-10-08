#pragma once

#ifndef	BENCHMARK_BUFFER
#define	BENCHMARK_BUFFER	0
#endif

#ifndef	BENCHMARK_ROUNDS
#define	BENCHMARK_ROUNDS	100000
#endif

#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define inline	inline __attribute__ ((always_inline))

void inline *benchmark_setup (void *p);
void benchmark_run (const void *restrict b, void *restrict a);

#define	BENCHMARK_SETUP(b)	void inline *benchmark_setup (void *b)
#define	BENCHMARK_RUN(b,a)	void benchmark_run (const void *restrict b, const void *restrict a) __attribute__ ((noinline))

// returns ns
static inline uint64_t rnow (void) {
	struct timespec t;
	clock_gettime (CLOCK_THREAD_CPUTIME_ID, &t);

	uint64_t r = t.tv_nsec;
	r += (1000 * 1000 * 1000) * t.tv_sec;

	return r;
}

uint8_t buf[BENCHMARK_BUFFER] = { 0 };

int main (int argc, char **argv) {
	void *b = buf;
	void *a = benchmark_setup (b);

	// get rnow hot
	int x;
	for (x = 0; x < BENCHMARK_ROUNDS; x++)
		rnow ();

	// simulate timing w/ reload
	volatile const uint64_t c0 = rnow ();
	const uint64_t c1 = rnow ();
	const uint64_t c = llabs (c1 - c0);

	uint64_t min = -1, max = 0;
	double avg = 0, var = 0;

	for (x = 0; x < (BENCHMARK_ROUNDS * 2); x++) {
		volatile const uint64_t start = rnow ();

		benchmark_run (b, a);

		const uint64_t end = rnow ();
		const uint64_t t = llabs (end - start) - c;

		const int valid = x >= BENCHMARK_ROUNDS;

		const typeof (t) nt = valid ? t : min, xt = valid ? t : max;
		min = min < nt ? min : nt;
		max = max > xt ? max : xt;

		// original algo from WP used d, d2
		const double ft = t;
		const double n = x + (double) (1 - BENCHMARK_ROUNDS);
		const double d0 = ft - avg;
		avg += valid ? d0 / n : 0;
		const double d2 = ft - avg;
		var += valid ? d0 * d2 : 0;

//		printf ("min (ns):	%"PRIu64"\n"
//			"max (ns):	%"PRIu64"\n"
//			"avg (ns):	%f\n"
//			"t (ns):	%"PRIu64"\n"
//			"c (ns):	%"PRIu64"\n\n\n",
//			 min, max, avg, t, c
//		);
	}

	var /= BENCHMARK_ROUNDS - 1;
	const double std = sqrt (var);

	printf ("rounds (n):	%i\n"
		"min (ns):	%"PRIu64"\n"
		"max (ns):	%"PRIu64"\n"
		"avg (ns):	%f\n"
		"std (ns):	%f\n",
		(int) BENCHMARK_ROUNDS, min, max, avg, std
	);

	return std;
}

#ifdef	TEST

int axb4;
BENCHMARK_RUN (a,b) {
	int x;
	for (x = 0; x < 1000000; x++)
		__sync_add_and_fetch (&axb4, x);

	return;
}

BENCHMARK_SETUP (a) {
	return NULL;
}

#endif
