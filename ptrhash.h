#ifndef PTRHASH_H_
#define PTRHASH_H_

#include <stdint.h>

static inline uint32_t ptrhash32_(uintptr_t hash)
{
	hash = ~hash + (hash << 15);
	hash = hash ^ (hash >> 12);
	hash = hash + (hash << 2);
	hash = hash ^ (hash >> 4);
	hash = hash * 2057;
	hash = hash ^ (hash >> 16);
	return (uint32_t)hash;
}

static inline uint32_t ptrhash64_(uintptr_t hash)
{
	hash = ~hash + (hash << 18);
	hash = hash ^ (hash >> 31);
	hash = hash * 21;
	hash = hash ^ (hash >> 11);
	hash = hash + (hash << 6);
	hash = hash ^ (hash >> 22);
	return (uint32_t)hash;
}

static inline uint32_t ptrhash(const void *ptr)
{
	uint32_t (*const hash_fn)(uintptr_t) =
		sizeof (uintptr_t) == sizeof (uint32_t) ? ptrhash32_ :
		sizeof (uintptr_t) == sizeof (uint64_t) ? ptrhash64_ :
		0;

	return hash_fn((uintptr_t)ptr);
}

#endif
