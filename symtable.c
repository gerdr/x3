#include "x3.h"
#include "murmur3.h"
#include "bithacks.h"

#include <stdlib.h>
#include <string.h>

/*
static inline size_t preferred_size(size_t load)
{
	// roughly corresponds to a load factor of 0.72,
	// the value recommended by Microsoft
	return next_greater_pow2((load * 11) / 8);
}*/

static x3_symbol *create_symbol(const char *name, void *value, uint32_t seed)
{
	size_t length = strlen(name);
	assert(length <= (uint32_t)-1);

	x3_symbol *symbol = malloc(sizeof *symbol + length + 1);
	if(!symbol) return NULL;

	memcpy(symbol->name, name, length + 1);
	symbol->hash = murmur3_32(symbol->name, length, seed);
	symbol->length = (uint32_t)length;
	symbol->value = value;

	return symbol;
}

//static _Bool reserve_symbols(x3_vm *vm, size_t count);

const x3_symbol *x3_define(x3_vm *vm, const char *name, void *value)
{
	x3_symbol *symbol = create_symbol(name, value, vm->symtable.seed);
	assert(!"TODO: add symbol to table");
	return symbol;
}

_Bool x3_init_symtable(x3_vm *vm, size_t size, uint32_t seed)
{
	assert(size > 0 && is_pow2z(size));
	assert((uint32_t)(size - 1) == size - 1);

	x3_symtable st = {
		.mask = (uint32_t)(size - 1),
		.seed = seed,
		.index = calloc(size, sizeof *st.index),
		.buckets = malloc(size * sizeof *st.buckets)
	};

	if(!st.index || !st.buckets)
		goto FAIL;

	vm->symtable = st;
	return 1;

FAIL:
	free(st.index);
	free(st.buckets);

	return 0;

}

/*
size_t m0_interp_chunk_map_size(const m0_interp *interp)
{
	const struct map *map = (struct map *)m0_interp_chunk_map(interp);
	return map->mask + 1;
}

static uint32_t hash_string(const m0_string *string, uint32_t seed)
{
	return murmur3_32(string->bytes, string->byte_size - sizeof *string, seed);
}

static bool register_chunk(struct map *map, uint32_t hash, uint32_t chunk_id)
{
	uint32_t slot = hash & map->mask;

	size_t fill_count = map->index[slot];
	assert(fill_count < (uint8_t)-1);

	if(fill_count == 0)
	{
		map->buckets[slot].as_single.hash = hash;
		map->buckets[slot].as_single.chunk_id = chunk_id;
		map->index[slot] = 1;
		return 1;
	}

	if(fill_count == 1)
	{
		struct bucket *bucket = (struct bucket *)malloc(sizeof *bucket);
		if(!bucket) return 0;

		bucket->hash = hash;
		bucket->chunk_id = chunk_id;

		map->buckets[slot].as_multiple = bucket;
		map->index[slot] = 2;

		return 1;
	}

	struct bucket *buckets = (struct bucket *)realloc(
		map->buckets[slot].as_multiple, (fill_count + 1) * sizeof *buckets);

	if(!buckets) return 0;

	buckets[fill_count].hash = hash;
	buckets[fill_count].chunk_id = (uint32_t)chunk_id;

	map->buckets[slot].as_multiple = buckets;
	map->index[slot] = (uint8_t)(fill_count + 1);

	return 1;
}

bool m0_interp_register_reserved_chunk(
	m0_interp *interp, const m0_string *name, size_t chunk_id)
{
	assert(chunk_id <= (uint32_t)-1);

	struct map *map = (struct map *)m0_interp_chunk_map(interp);
	uint32_t hash = hash_string(name, map->seed);

	return register_chunk(map, hash, (uint32_t)chunk_id);
}

bool m0_interp_reserve_chunk_map_slots(m0_interp *interp, size_t count)
{
	struct map *map = (struct map *)m0_interp_chunk_map(interp);
	size_t current_size = map->mask + 1;

	size_t new_load = m0_interp_chunk_count(interp) + count;
	size_t new_size = preferred_map_size(new_load);

	if(current_size == new_size)
		return 1;

	uint32_t new_mask = (uint32_t)(new_size - 1);
	assert(new_mask == new_size - 1);

	struct map *new_map = (struct map *)malloc(
		sizeof *new_map + new_size * sizeof *new_map->buckets);

	uint8_t *new_index = (uint8_t *)calloc(new_size, sizeof *new_index);

	if(!new_map || !new_index)
		goto FAIL;

	new_map->index = new_index;
	new_map->mask = new_mask;
	new_map->seed = map->seed;

	for(size_t i = 0; i < current_size; ++i)
	{
		size_t fill_count = map->index[i];
		if(!fill_count) continue;

		if(fill_count == 1)
		{
			struct bucket *bucket = &map->buckets[i].as_single;
			if(!register_chunk(map, bucket->hash, bucket->chunk_id))
				goto DESTROY_AND_FAIL;

			continue;
		}

		for(size_t j = 0; j < fill_count; ++j)
		{
			struct bucket *bucket = &map->buckets[i].as_multiple[j];
			if(!register_chunk(map, bucket->hash, bucket->chunk_id))
				goto DESTROY_AND_FAIL;

			continue;
		}
	}

	m0_interp_set_chunk_map(interp, new_map);

	return 1;

DESTROY_AND_FAIL:

	for(size_t i = 0; i < new_size; ++i)
	{
		if(new_index[i] > 1)
			free(new_map->buckets[i].as_multiple);
	}

FAIL:

	free(new_index);
	free(new_map);

	return 0;
}
*/
