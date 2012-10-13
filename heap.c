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

static bool add_entry(x3_heap *hp, const x3_meta *meta)
{
	uint32_t slot = ptrhash(meta->body) & hp->mask;
	size_t fill_count = hp->index[slot];
	assert(fill_count < (uint8_t)-1);

	x3_meta *entries = hp->buckets[slot];

	// check for duplicate entries
	for(size_t i = 0; i < fill_count; ++i)
	{
		if(hp->buckets[slot][i].body == meta->body)
			return 0;
	}

	if(fill_count == 0)
		hp->buckets[slot] = NULL;

	entries = realloc(entries, (fill_count + 1) * sizeof *entries);
	if(!entries) return 0;

	entries[fill_count] = *meta;

	hp->buckets[slot] = entries;
	hp->index[slot] = (uint8_t)(fill_count + 1);
	++hp->load;

	return 1;
}

static void try_rehashing_if_necessary(x3_heap *hp, size_t count)
{
	size_t current_size = hp->mask + 1;
	size_t new_load = hp->load + count;
	size_t new_size = preferred_size(new_load);

	if(new_size <= current_size)
		return;

	uint32_t new_mask = (uint32_t)(new_size - 1);
	assert(new_mask == new_size - 1);

	x3_meta **new_buckets = malloc(new_size * sizeof *new_buckets);
	uint8_t *new_index = calloc(new_size, sizeof *new_index);

	if(!new_buckets || !new_index)
		goto FAIL;

	x3_heap new_hp = {
		.index = new_index,
		.mask = new_mask,
		.load = 0,
		.buckets = new_buckets
	};

	for(size_t i = 0; i < current_size; ++i)
	{
		for(size_t j = 0; j < hp->index[i]; ++j)
		{
			if(!add_entry(hp, &hp->buckets[i][j]))
				goto DESTROY_AND_FAIL;
		}
	}

	*hp = new_hp;
	return;

DESTROY_AND_FAIL:

	for(size_t i = 0; i < new_size; ++i)
	{
		if(new_index[i])
			free(new_buckets[i]);
	}

FAIL:

	free(new_buckets);
	free(new_index);
}

bool x3_register(x3_vm *vm, void *obj, void *metadata, x3_dispatcher dp)
{
	x3_meta meta = {
		.body = obj,
		.data = metadata,
		.dispatcher = dp
	};

	try_rehashing_if_necessary(&vm->heap, 1);
	return add_entry(&vm->heap, &meta);
}
