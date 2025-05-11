// SPDX-License-Identifier: GPL-2.0

#include "alloc_cache.h"

/**
 * Iterates through all cached entries, freeing each one using the provided
 * free function, then releases the memory used for the cache array itself.
 * Sets the cache entries pointer to NULL after freeing.
 */
void io_alloc_cache_free(struct io_alloc_cache *cache,
			 void (*free)(const void *))
{
	void *entry;

	if (!cache->entries)
		return;

	while ((entry = io_alloc_cache_get(cache)) != NULL)
		free(entry);

	kvfree(cache->entries);
	cache->entries = NULL;
}

/**
 * Allocates memory for the cache entry array and sets up cache parameters.
 * Returns false if initialization succeeds, true if memory allocation fails.
 */
bool io_alloc_cache_init(struct io_alloc_cache *cache,
			 unsigned max_nr, unsigned int size,
			 unsigned int init_bytes)
{
	cache->entries = kvmalloc_array(max_nr, sizeof(void *), GFP_KERNEL);
	if (!cache->entries)
		return true;

	cache->nr_cached = 0;
	cache->max_cached = max_nr;
	cache->elem_size = size;
	cache->init_clear = init_bytes;
	return false;
}

/**
 * Allocates a new object of the cache's element size. If init_clear is set,
 * the allocated memory is zeroed for the specified number of bytes.
 * Returns a pointer to the new object or NULL on failure.
 */
void *io_cache_alloc_new(struct io_alloc_cache *cache, gfp_t gfp)
{
	void *obj;

	obj = kmalloc(cache->elem_size, gfp);
	if (obj && cache->init_clear)
		memset(obj, 0, cache->init_clear);
	return obj;
}
