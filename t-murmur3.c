#include "murmur3.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#undef NDEBUG
#include <assert.h>

enum { SEED = 42 };

#define LOREM_IPSUM \
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do " \
	"eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad " \
	"minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip " \
	"ex ea commodo consequat. Duis aute irure dolor in reprehenderit in " \
	"voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur " \
	"sint occaecat cupidatat non proident, sunt in culpa qui officia " \
	"deserunt mollit anim id est laborum."

int main(void)
{
	static const char TEXT[] = LOREM_IPSUM;
	static const char *const TEXT_END = (&TEXT)[1];
	static uint32_t buffer[(sizeof TEXT + 3) / 4];

	assert(sizeof (uint32_t) == 4);
	assert(sizeof buffer >= sizeof TEXT);

	for(const char *cursor = TEXT; cursor < TEXT_END; ++cursor)
	{
		size_t size = (size_t)(TEXT_END - cursor);
		memcpy(buffer, cursor, size);
		assert(murmur3_32(cursor, size, SEED) ==
			murmur3_32(buffer, size, SEED));
	}

	return 0;
}
