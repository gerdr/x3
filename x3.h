#ifndef X3_H_
#define X3_H_

#include <stddef.h>
#include <stdint.h>

typedef int64_t x3_int;
typedef uint64_t x3_uint;
typedef double x3_float;
typedef ptrdiff_t x3_word;
typedef size_t x3_uword;

enum x3_msg_
{
	X3_MSG_GET,
	X3_MSG_SET,
	X3_MSG_CALL
};

typedef enum x3_msg_ x3_msg;
typedef union x3_value_ x3_value;
typedef struct x3_vm_ x3_vm;
typedef void x3_dispatcher(x3_vm *, void *, void *, x3_msg, void *);

void x3_core(x3_vm *vm);
void x3_dispatch(x3_vm *vm, void *obj, x3_msg msg, void *args);
void x3_register(x3_vm *vm, void *obj, void *meta, x3_dispatcher dp);
void x3_gc(x3_vm *vm);
void x3_define(x3_vm *vm, const char *name, x3_value value, _Bool is_ref);
void x3_undef(x3_vm *vm, const char *name);
const x3_value *x3_resolve(x3_vm *vm, const char *name);

// HERE BE DRAGONS

typedef struct x3_callframe_ x3_callframe;
typedef struct x3_callstack_ x3_callstack;
typedef struct x3_object_ x3_object;

union x3_value_
{
	x3_int as_int;
	x3_uint as_uint;
	x3_float as_float;
	x3_word as_word;
	x3_uword as_uword;
	void *as_ptr;
	const void *as_cptr;
	void (*as_fptr)(void);
};

struct x3_object_
{
	x3_dispatcher *dispatcher;
	void *body;
	void *meta;
};

struct x3_callframe_
{
	const x3_value *ip;
	void *fp;
};

struct x3_callstack_
{
	x3_callframe *top, *floor, *ceil;
};

struct x3_vm_
{
	x3_callstack callstack;
};

#endif
