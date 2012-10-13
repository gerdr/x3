#include "x3.h"
#include <assert.h>

#define DEF(NAME) \
	static const void *const core_ ## NAME ## _ = &&core_ ## NAME; \
	(void)core_ ## NAME ## _; core_ ## NAME: \
	__asm__ __volatile__ (".global x3_core_" #NAME "\nx3_core_" #NAME ":")

#define CURRENT(TYPE) ip->as_ ## TYPE
#define NEXT(TYPE) (++ip)->as_ ## TYPE

void x3_core(x3_vm *vm)
{
	register const x3_corecell *ip;
	register void *fp;

	register union { x3_int s; x3_uint u; } ia, ib;
	register union { x3_float f; } fa, fb;
	register union { char *p; } pa, pb;

	x3_callstack *const cs = &vm->callstack;

	DEF(return);
	assert(cs->top > cs->floor);
	--cs->top;
	ip = cs->top->ip;
	fp = cs->top->fp;
	goto *CURRENT(cptr);

	DEF(add_ia);
	ia.u += ib.u;
	goto *NEXT(cptr);

	DEF(add_fa);
	fa.f += fb.f;
	goto *NEXT(cptr);

	DEF(offset_pa);
	pa.p += (x3_word)ia.s * NEXT(word);
	goto *NEXT(cptr);

	DEF(offset_pb);
	pb.p += (x3_word)ib.s * NEXT(word);
	goto *NEXT(cptr);

	DEF(yield);
	assert(cs->top < cs->ceil);
	cs->top->ip = ip + 1;
	cs->top->fp = fp;
	++cs->top;
}
