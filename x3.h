#ifndef X3_H_
#define X3_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int64_t x3_int;
typedef uint64_t x3_uint;
typedef double x3_float;
typedef ptrdiff_t x3_word;
typedef size_t x3_uword;

enum x3_msg_
{
	X3_MSG_NOOP,
	X3_MSG_GET,
	X3_MSG_SET,
	X3_MSG_CALL
};

typedef enum x3_msg_ x3_msg;
typedef struct x3_vm_ x3_vm;
typedef void x3_dispatcher(x3_vm *, void *, void *, x3_msg, void *);
typedef struct x3_symbol_ x3_symbol;

extern const x3_vm X3_VM;

extern void x3_core(x3_vm *vm);
extern bool x3_dispatch(x3_vm *vm, void *obj, x3_msg msg, void *args);
extern void x3_register(x3_vm *vm, void *obj, void *metadata, x3_dispatcher dp);
extern void x3_gc(x3_vm *vm);
extern const x3_symbol *x3_define(x3_vm *vm, const char *name, void *value);
extern void x3_undef(x3_vm *vm, const x3_symbol *symbol);
extern void *x3_resolve(x3_vm *vm, const char *name);
// TODO: functions to load/unload chunks

static inline size_t x3_symtable_size(x3_vm *vm);
static inline size_t x3_symtable_load(x3_vm *vm);

// HERE BE DRAGONS

typedef struct x3_callframe_ x3_callframe;
typedef struct x3_callstack_ x3_callstack;

typedef struct x3_meta_ x3_meta;
typedef union x3_heapbucket_ x3_heapbucket;
typedef struct x3_heap_ x3_heap;

typedef struct x3_symtable_ x3_symtable;

typedef union x3_corecell_ x3_corecell;

extern bool x3_init_symtable(x3_vm *vm, size_t size, uint32_t seed);
extern bool x3_init_heap(x3_vm *vm, size_t size);

union x3_corecell_
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

struct x3_symbol_
{
	x3_symbol *next;
	void *value;
	uint32_t hash;
	uint32_t length;
	uint8_t name[];
};

struct x3_symtable_
{
	uint32_t mask;
	uint32_t seed;
	size_t load;
	x3_symbol **buckets;
};

struct x3_heap_
{
	uint8_t *index;
	uint32_t mask;
	size_t load;
	x3_meta **buckets;
};

struct x3_meta_
{
	uintptr_t flags;
	x3_dispatcher *dispatcher;
	void *body;
	void *data;
};

struct x3_callframe_
{
	const x3_corecell *ip;
	void *fp;
};

struct x3_callstack_
{
	x3_callframe *top, *floor, *ceil;
};

struct x3_vm_
{
	x3_callstack callstack;
	x3_symtable symtable;
	x3_heap heap;
};

static inline size_t x3_symtable_size(x3_vm *vm)
{
	return (size_t)(vm->symtable.mask + 1);
}

static inline size_t x3_symtable_load(x3_vm *vm)
{
	return vm->symtable.load;
}

#endif
