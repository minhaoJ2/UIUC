#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define STRUCT_SIZE sizeof(metadata_entry_t)

static size_t capacity = 1024;
typedef struct _metadata_entry_t {
	size_t size;
	int free;
	struct _metadata_entry_t *next;
	struct _metadata_entry_t *prev;
	struct _metadata_entry_t *prev_free;
	struct _metadata_entry_t *next_free;
} metadata_entry_t;

void coalesce(metadata_entry_t *ptr1, metadata_entry_t *ptr2);
void *resize(size_t size);
void *split(metadata_entry_t *ptr, size_t size);
void removefromlist(metadata_entry_t *ptr);
void addtolist(metadata_entry_t *ptr);

static metadata_entry_t *head = NULL;
static metadata_entry_t *free_head = NULL;
static metadata_entry_t *end = NULL;
static metadata_entry_t *free_end = NULL;

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 * Number of elements to be allocated.
 * @param size
 * Size of elements.
 *
 * @return
 * A pointer to the memory block allocated by the function.
 *
 * The type of this pointer is always void*, which can be cast to the
 * desired type of data pointer in order to be dereferenceable.
 *
 * If the function failed to allocate the requested block of memory, a
 * NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
	// implement calloc!
    size_t total = num * size;
	void *res = malloc(total);
	if (res == NULL) {
	    return NULL;
    }
	memset(res, 0, total);
	return res;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block. The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 * Size of the memory block, in bytes.
 *
 * @return
 * On success, a pointer to the memory block allocated by the function.
 *
 * The type of this pointer is always void*, which can be cast to the
 * desired type of data pointer in order to be dereferenceable.
 *
 * If the function failed to allocate the requested block of memory,
 * a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
	// implement malloc!
    size = ALIGN(size);
	if (head == NULL) {
	    capacity = ALIGN(size + capacity - 1);
	    void* temp = sbrk(capacity);
        if (temp == (void*) -1) {
            return NULL;
        }
        head = (metadata_entry_t*)temp;
        head -> size = capacity - STRUCT_SIZE;
        head -> prev = NULL;
        head -> next = NULL;
        head -> prev_free = NULL;
        head -> next_free = NULL;
        head -> free = 0;
        end = head;
        free_head = head;
        free_end = head;
	}

	metadata_entry_t *p = free_head;
	int find = 0;
	while (p != NULL) {
		if (p -> free == 0 && p -> size >= size) {
			p -> free = 1;
			find = 1;
            void* result = (void*)(p + 1);
			if (p -> size - size > 2 * STRUCT_SIZE) {
				split(p, size);
			}
		    removefromlist(p);
			return result;
		}
		p = p -> next_free;
	}
	if (find == 0) {
	    void *res = resize(size);
	    if (res == NULL) {
	        return NULL;
        }
	    return malloc(size);
	}
	return NULL;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 * Pointer to a memory block previously allocated with malloc(),
 * calloc() or realloc() to be deallocated. If a null pointer is
 * passed as argument, no action occurs.
 */
void free(void *ptr) {
	// implement free!
	metadata_entry_t *temp = (metadata_entry_t *)ptr - 1;
	temp -> free = 0;
	int free1 = 0, free2 = 0;
	if (temp -> prev != NULL && temp -> prev -> free == 0) {
		free1 = 1;
	}
	if (temp -> next != NULL && temp -> next -> free == 0) {
		free2 = 1;
	}
	if (free2 == 0 && free1 == 0) {
		addtolist(temp);
	} else if (free1 == 0 && free2 == 1) {
		metadata_entry_t *nextblock = temp->next;
		removefromlist(nextblock);
		coalesce(temp, nextblock);
		addtolist(temp);
	} else if (free2 == 0 && free1 == 1) {
		metadata_entry_t *pre = temp->prev;
		removefromlist(pre);
		coalesce(pre, temp);
		addtolist(pre);
	} else {
		metadata_entry_t *pre = temp->prev;
		metadata_entry_t *next = temp->next;
		removefromlist(pre);
		coalesce(pre, temp);
		addtolist(pre);
		removefromlist(next);
		removefromlist(pre);
		coalesce(pre, next);
		addtolist(pre);
	}
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 * Pointer to a memory block previously allocated with malloc(), calloc()
 * or realloc() to be reallocated.
 *
 * If this is NULL, a new block is allocated and a pointer to it is
 * returned by the function.
 *
 * @param size
 * New size for the memory block, in bytes.
 *
 * If it is 0 and ptr points to an existing block of memory, the memory
 * block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 * A pointer to the reallocated memory block, which may be either the
 * same as the ptr argument or a new location.
 *
 * The type of this pointer is void*, which can be cast to the desired
 * type of data pointer in order to be dereferenceable.
 *
 * If the function failed to allocate the requested block of memory,
 * a NULL pointer is returned, and the memory block pointed to by
 * argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
	// implement realloc!
	if (ptr == NULL) {
	    return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    metadata_entry_t* entry = (metadata_entry_t*)ptr - 1;
    if (entry -> size >= size) {
        void* result = (void*)(entry + 1);
        return result;
    } else {
        metadata_entry_t* result = NULL;
        metadata_entry_t* temp = entry -> next;
        if (temp == NULL || temp -> free == 1 || entry -> size + temp -> size < size - STRUCT_SIZE) {
            result = malloc(size);
            if (result == NULL) {
                return NULL;
            } else {
                memcpy(result, entry + 1, entry -> size);
                free(entry + 1);
                return result;
            }
        } else {
            removefromlist(temp);
            entry -> size += temp -> size + STRUCT_SIZE;
            entry -> next = temp -> next;
            if (temp -> next != NULL) {
                temp -> next -> prev = entry;
            } else {
                end = entry;
            }
            if (entry -> size - size > 2 * STRUCT_SIZE) {
                split(entry, size);
            }
            void* retval = (void*)(entry + 1);
            return retval;
        }
    }
    return NULL;
}

void *resize(size_t size) {
	size_t total = ALIGN(size + STRUCT_SIZE + capacity - 1);
    void* temp = sbrk(total);
	if (temp == (void *) -1) {
	    return NULL;
    }
    metadata_entry_t *result = (metadata_entry_t*)temp;
	result -> size = total - STRUCT_SIZE;
	result -> free = 0;
	result -> prev = end;
	result -> next = NULL;
	end -> next = result;
	addtolist(result);
	end = result;
	capacity += total;
	return result;
}

void *split(metadata_entry_t *ptr, size_t size) {
	metadata_entry_t *temp = (metadata_entry_t *)((char *)(ptr + 1) + size);
	temp -> free = 0;
	temp -> size = ptr -> size - size - STRUCT_SIZE;
	temp -> prev = ptr;
	temp -> next = ptr->next;
	temp -> next_free = NULL;
	temp -> prev_free = NULL;
	ptr -> next = temp;
	ptr -> size = size;
	if (ptr != end) {
	    temp -> next -> prev = temp;
    } else {
	    end = temp;
    }
	addtolist(temp);
	return temp;
}

void coalesce(metadata_entry_t *current, metadata_entry_t *next) {
	current -> size  = current -> size + next -> size + STRUCT_SIZE;
	current -> free = 0;
	current -> next = next -> next;
	if (next != end) {
	    next -> next -> prev = current;
	} else {
	    end = current;
	}
}

void removefromlist(metadata_entry_t *ptr) {
	ptr->free = 1;
	if (ptr == free_head && ptr == free_end) {
	    free_head = NULL;
	    free_end = NULL;
	    ptr->prev_free = NULL;
	    ptr->next_free = NULL;
	    return;
	}
	if (ptr == free_head) {
	    free_head = ptr -> next_free;
    } else {
	    ptr -> prev_free -> next_free = ptr -> next_free;
    }

	if (ptr == free_end) {
	    free_end = ptr -> prev_free;
    } else {
	    ptr -> next_free -> prev_free = ptr -> prev_free;
    }

	ptr -> prev_free = NULL;
	ptr -> next_free = NULL;
}

void addtolist(metadata_entry_t *ptr) {
	ptr -> free = 0;
	if (free_head == NULL) {
	    free_end = ptr;
	    free_head = ptr;
	    ptr -> next_free = NULL;
	    ptr -> prev_free = NULL;
	    return;
	}
	free_end -> next_free = ptr;
	ptr -> prev_free = free_end;
	ptr -> next_free = NULL;
	free_end = ptr;
}
