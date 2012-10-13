#include "x3.h"

#undef NDEBUG
#include <assert.h>

enum { SIZE = 64 };

int main(void)
{
	x3_vm vm[] = { X3_VM };

	assert(x3_init_heap(vm, SIZE));

	return 0;
}
