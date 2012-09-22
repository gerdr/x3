#ifndef BITHACKS_H_
#define BITHACKS_H_

#include <limits.h>
#include <stddef.h>

static inline size_t is_pow2z(size_t size)
{
	return (size & (size - 1)) == 0;
}

static inline size_t next_greater_pow2(size_t size)
{
	enum { SIZE_BIT = CHAR_BIT * sizeof (size_t) };

	for(unsigned exp = 0; (1 << exp) < SIZE_BIT; ++exp)
		size |= size >> (1 << exp);

	return size + 1;
}

#endif
