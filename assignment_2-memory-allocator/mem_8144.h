#ifndef _MEM_8144_H
#define _MEM_8144_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

/* total size of the memory region */
#define MEM_SIZE 32*1024*1024

/* maximum order of memory region */
#define MEM_MAX_ORDER 25

/* padding for buddy algorithm to match with memory order */
#define BUDDY_ORDER_PADDING 15

/* maximum order of buddy algorithm */
#define BUDDY_MAX_ORDER 10

/* minimal object size of a slab area: 32 bytes */
#define SLAB_MIN_ORDER 5

/* maximal object size of a slab area: 2^14 bytes */
#define SLAB_MAX_ORDER 14

/* size of slab cache list */
#define CACHE_LIST_SIZE 10

/* all slabs have the same size 128 * 1024 (128K) */
#define SLAB_SIZE 128*1024

/* the address of the buddy of a block from buddy_lists[i]. */
#define _MEMBASE(base)        ((uintptr_t) base)
#define _OFFSET(base, b)      ((uintptr_t)b - _MEMBASE(base))
#define _BUDDYOF(base, b, i)  (_OFFSET(base, b) ^ (1 << (i)))
#define BUDDY_OF(base, b, i)   ((pointer)( _BUDDYOF(base, b, i) + _MEMBASE(base)))

/* used for untyped pointers */
typedef void *pointer;

/* pointers to the free space lists */
pointer buddy_lists[BUDDY_MAX_ORDER + 1];

/* the start of managed memory */
static pointer mem_region_ptr = NULL;

/* list of slab caches, each index contain different size objects
 * index [0] contains 2^5 size object
 * index [1] contains 2^6 size object
 * index [2] contains 2^7 size object
 * index [9] contains 2^14 size object*/
struct slab_header *cache_list[CACHE_LIST_SIZE];

/* index i contains value of 2^i */
int pow_of_two[MEM_MAX_ORDER + 1];

/* stores internal fragmentation value */
static long long int global_internal_fragmentation;

/* stores external fragmentation value */
static long long int global_external_fragmentation;

/* shared resource access lock to support concurrency */
pthread_mutex_t global_mutex_lock;

/* intended for coloring slabs, not in use right now */
typedef enum slab_color {
    EMPTY,
    PARTIAL,
    FULL
} COLOR;

/* store information about memory pointer */
struct obj_header {
    pointer block;
    unsigned is_free;
    struct obj_header *next;
};

/* store information regarding slab */
struct slab_header {
    pointer mem_base;
    int total_objects;
    struct obj_header *obj_head;
    struct slab_header *next;
    struct slab_header *previous;
};

/* used to track memory allocation and de-allocation */
struct mem_ptr {
    int alloc_size;
    int alloc_id;
    pointer block;
};

/* initialization */
void kmem_init();

/* malloc a continuous memory spacce */
pointer kmalloc_8144(int size);

/* free the allocated memory space */
void kfree_8144(pointer ptr);

/* purge the free slabs */
void purge_8144();

/* report the size of internal fragmentations in bytes */
long long int internal_frag();

/* report the size of external fragmentations (holes) in bytes */
long long int external_frag();

/* destructor */
void kmem_finit();

/* print buddy list */
void print_buddy();

/* print a single slab */
void print_slab(int slab_order);

#endif
