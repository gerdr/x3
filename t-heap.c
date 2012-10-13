#include "x3.h"

#undef NDEBUG
#include <assert.h>

enum { SIZE = 64 };

int main(void)
{
	x3_vm vm[] = { X3_VM };

	assert(x3_init_heap(vm, SIZE));
	assert(x3_heap_size(vm) == SIZE);
	assert(x3_heap_load(vm) == 0);

	return 0;
}
