#include "../aead.h"

BENCHMARK_RUN (ip, op) {
	const input *i = ip;

	int e;
	e = crypto_aead_xchacha20poly1305_ietf_encrypt (op, NULL,
						i->pt, sizeof (i->pt),
						i->d, sizeof (i->d),
						NULL, i->d, i->k
	);

	assert (!e);
	return;
}
