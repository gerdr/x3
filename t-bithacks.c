#include "bithacks.h"

#undef NDEBUG
#include <assert.h>

int main(void)
{
	{
		assert(is_pow2z(0) == 1);
		assert(is_pow2z(1) == 1);
		assert(is_pow2z(2) == 1);
		assert(is_pow2z(3) == 0);
		assert(is_pow2z(4) == 1);
		assert(is_pow2z(5) == 0);
		assert(is_pow2z(1023) == 0);
		assert(is_pow2z(1024) == 1);
		assert(is_pow2z((size_t)-1 / 2 + 1) == 1);
		assert(is_pow2z((size_t)-1) == 0);
	}

	{
		assert(next_greater_pow2(0) == 1);
		assert(next_greater_pow2(1) == 2);
		assert(next_greater_pow2(2) == 4);
		assert(next_greater_pow2(3) == 4);
		assert(next_greater_pow2(4) == 8);
		assert(next_greater_pow2(5) == 8);
		assert(next_greater_pow2(1023) == 1024);
		assert(next_greater_pow2(1024) == 2048);
		assert(next_greater_pow2((size_t)-1 / 2) == (size_t)-1 / 2 + 1);
		assert(next_greater_pow2((size_t)-1 / 2 + 1) == 0);
	}

	return 0;
}
