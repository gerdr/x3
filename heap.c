#include "x3.h"
#include "ptrhash.h"
#include "bithacks.h"

#include <assert.h>
#include <stdlib.h>

static inline size_t preferred_size(size_t load)
{
	// roughly corresponds to a load factor of 0.72,
	// the value recommended by Microsoft
	return next_greater_pow2((load * 11) / 8);
}

static x3_meta *get_meta(const x3_heap *hp, const void *obj)
{
	uint32_t slot = ptrhash(obj) & hp->mask;

	for(size_t i = 0; i < hp->index[slot]; ++i)
	{
		x3_meta *meta = hp->buckets[slot] + i;
		if(meta->body == obj)
			return meta;
	}

	return NULL;
}

bool x3_dispatch(x3_vm *vm, void *obj, x3_msg msg, void *args)
{
	x3_meta *meta = get_meta(&vm->heap, obj);
	if(!meta) return 0;

	meta->dispatcher(vm, obj, meta->data, msg, args);
	return 1;
}

bool x3_init_heap(x3_vm *vm, size_t size)
{
	assert(size > 0 && is_pow2z(size));
	assert((uint32_t)(size - 1) == size - 1);

	x3_heap hp = {
		.mask = (uint32_t)(size - 1),
		.load = 0,
		.index = calloc(size, sizeof *hp.index),
		.buckets = malloc(size * sizeof *hp.buckets)
	};

	if(!hp.index || !hp.buckets)
		goto FAIL;

	vm->heap = hp;
	return 1;

FAIL:
	free(hp.index);
	free(hp.buckets);
	return 0;
}

/*

TODO: reuse old symtable implementation

static _Bool register_symbol(x3_symtable *st, x3_symbol *symbol)
{
	uint32_t slot = symbol->hash & st->mask;

	size_t fill_count = st->index[slot];
	assert(fill_count < (uint8_t)-1);

	if(fill_count == 0)
	{
		st->buckets[slot].as_single = symbol;
		st->index[slot] = 1;
		++st->load;

		return 1;
	}

	if(fill_count == 1)
	{
		x3_symbol *other_symbol = st->buckets[slot].as_single;
		if(equal_names(symbol, other_symbol))
			return 0;

		x3_symbol **symbols = malloc(2 * sizeof *symbols);
		if(!symbols) return 0;

		symbols[0] = other_symbol;
		symbols[1] = symbol;

		st->buckets[slot].as_multiple = symbols;
		st->index[slot] = 2;
		++st->load;

		return 1;
	}

	x3_symbol **symbols = st->buckets[slot].as_multiple;

	// fail on symbol duplication
	for(unsigned i = 0; i < fill_count; ++i)
	{
		if(equal_names(symbol, symbols[i]))
			return 0;
	}

	symbols = realloc(symbols, (fill_count + 1) * sizeof *symbols);
	if(!symbols) return 0;

	symbols[fill_count] = symbol;

	st->buckets[slot].as_multiple = symbols;
	st->index[slot] = (uint8_t)(fill_count + 1);
	++st->load;

	return 1;
}

static void try_rehashing_if_necessary(x3_symtable *st, size_t count)
{
	size_t current_size = st->mask + 1;
	size_t new_load = st->load + count;
	size_t new_size = preferred_size(new_load);

	if(new_size <= current_size)
		return;

	uint32_t new_mask = (uint32_t)(new_size - 1);
	assert(new_mask == new_size - 1);

	x3_symbucket *new_buckets = malloc(
		sizeof *new_buckets + new_size * sizeof *new_buckets);

	uint8_t *new_index = calloc(new_size, sizeof *new_index);

	if(!new_buckets || !new_index)
		goto FAIL;

	x3_symtable new_st = {
		.index = new_index,
		.mask = new_mask,
		.seed = st->seed,
		.load = 0,
		.buckets = new_buckets
	};

	for(size_t i = 0; i < current_size; ++i)
	{
		size_t fill_count = st->index[i];
		if(!fill_count) continue;

		if(fill_count == 1)
		{
			if(!register_symbol(&new_st, st->buckets[i].as_single))
				goto DESTROY_AND_FAIL;

			continue;
		}

		for(size_t j = 0; j < fill_count; ++j)
		{
			x3_symbol *symbol = st->buckets[i].as_multiple[j];
			if(!register_symbol(&new_st, symbol))
				goto DESTROY_AND_FAIL;

			continue;
		}
	}

	*st = new_st;

	return;

DESTROY_AND_FAIL:

	for(size_t i = 0; i < new_size; ++i)
	{
		if(new_index[i] > 1)
			free(new_buckets[i].as_multiple);
	}

FAIL:

	free(new_buckets);
	free(new_index);
}

const x3_symbol *x3_define(x3_vm *vm, const char *name, void *value)
{
	x3_symbol *symbol = create_symbol(name, value, vm->symtable.seed);
	if(!symbol)
		return NULL;

	try_rehashing_if_necessary(&vm->symtable, 1);

	if(!register_symbol(&vm->symtable, symbol))
		goto FAIL;

	return symbol;

FAIL:

	free(symbol);

	return NULL;
}
*/
