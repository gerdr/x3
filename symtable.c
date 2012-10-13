#include "x3.h"
#include "murmur3.h"
#include "bithacks.h"

#include <stdlib.h>
#include <string.h>

static x3_symbol *create_symbol(const char *name, void *value, uint32_t seed)
{
	size_t length = strlen(name);
	assert(length <= (uint32_t)-1);

	x3_symbol *symbol = malloc(sizeof *symbol + length + 1);
	if(!symbol) return NULL;

	memcpy(symbol->name, name, length + 1);
	symbol->next = NULL;
	symbol->hash = murmur3_32(symbol->name, length, seed);
	symbol->length = (uint32_t)length;
	symbol->value = value;

	return symbol;
}

static _Bool equal_names(const x3_symbol *s1, const x3_symbol *s2)
{
	return s1->hash == s2->hash &&
		strcmp((char *)s1->name, (char *)s2->name) == 0;
}

static _Bool register_symbol(x3_symtable *st, x3_symbol *symbol)
{
	uint32_t slot = symbol->hash & st->mask;

	// fail on symbol duplication
	const x3_symbol *current = st->buckets[slot];
	for(; current; current = current->next)
	{
		assert(current);
		assert(symbol);
		if(equal_names(symbol, current))
			return 0;
	}

	symbol->next = st->buckets[slot];
	st->buckets[slot] = symbol;
	++st->load;

	return 1;
}

static inline size_t preferred_size(size_t load)
{
	// roughly corresponds to a load factor of 0.72,
	// the value recommended by Microsoft
	return next_greater_pow2((load * 11) / 8);
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

	x3_symbol **new_buckets = calloc(new_size, sizeof *new_buckets);

	if(!new_buckets)
		goto FAIL;

	x3_symtable new_st = {
		.mask = new_mask,
		.seed = st->seed,
		.load = 0,
		.buckets = new_buckets
	};

	for(size_t i = 0; i < current_size; ++i)
	{
		x3_symbol *current = st->buckets[i];
		for(x3_symbol *next; current; current = next)
		{
			next = current->next;
			if(!register_symbol(&new_st, st->buckets[i]))
				goto DESTROY_AND_FAIL;
		}
	}

	*st = new_st;
	return;

DESTROY_AND_FAIL:
	for(size_t i = 0; i < new_size; ++i)
		free(new_buckets[i]);

FAIL:
	free(new_buckets);
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

_Bool x3_init_symtable(x3_vm *vm, size_t size, uint32_t seed)
{
	assert(size > 0 && is_pow2z(size));
	assert((uint32_t)(size - 1) == size - 1);

	x3_symtable st = {
		.mask = (uint32_t)(size - 1),
		.seed = seed,
		.load = 0,
		.buckets = calloc(size, sizeof *st.buckets)
	};

	if(!st.buckets)
		goto FAIL;

	vm->symtable = st;

	return 1;

FAIL:
	free(st.buckets);
	return 0;
}

void *x3_resolve(x3_vm *vm, const char *name)
{
	x3_symtable *const st = &vm->symtable;

	uint32_t hash = murmur3_32(name, strlen(name), st->seed);
	uint32_t slot = hash & st->mask;

	const x3_symbol *current = st->buckets[slot];
	for(; current; current = current->next)
	{
		if(current->hash == hash && strcmp((char *)current->name, name) == 0)
			return current->value;
	}

	return NULL;
}
