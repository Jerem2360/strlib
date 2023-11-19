#include "strhash.h"
#include <cmath>


hash_t computeHash(const char* str, size_t sz) {
	hash_t res = 0;
	for (int i = 0; i < sz; i++) {
		res += (hash_t)(str[i] * pow(31, i));
	}
	return res;
}

