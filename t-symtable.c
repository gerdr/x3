#include "x3.h"

#undef NDEBUG
#include <assert.h>

enum { SEED = 42, SIZE = 64 };

int main(void)
{
	x3_vm vm[] = { X3_VM };

	assert(x3_init_symtable(vm, SIZE, SEED));
	assert(x3_symtable_size(vm) == SIZE);
	assert(x3_symtable_load(vm) == 0);

	{
		const x3_symbol *symbol = x3_define(vm, "FOO", NULL);
		assert(symbol != NULL);
		assert(x3_symtable_load(vm) == 1);
	}

	return 0;
}
