#include "x3.h"

#undef NDEBUG
#include <assert.h>

int main(void)
{
	x3_vm vm[] = { X3_VM };
	x3_callframe frames[64];

	x3_callstack *const cs = &vm->callstack;
	cs->floor = frames;
	cs->top = frames;
	cs->ceil = (&frames)[1];

	{
		const x3_corecell cells[] = { { .as_cptr = x3_core_yield } };
		cs->top->ip = cells;
		cs->top->fp = NULL;
		++cs->top;

		x3_core(vm);
		assert(cs->top == cs->floor + 1);
		assert(cs->top[-1].ip = cells + 1);
	}

	return 0;
}
