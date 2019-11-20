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

/* Total size of the memory region */
#define MEM_SIZE 32*1024*1024

/* maximal order of buddy algorithm */
#define BUDDY_MAX_ORDER 25

/* minimal object size of a slab area: 32 bytes */
#define SLAB_MIN_ORDER 5

/* maximal object size of a slab area: 2^14 bytes */
#define SLAB_MAX_ORDER 14

/* size of slab cache list */
#define CACHE_LIST_SIZE 10

/* all slabs have the same size 128 * 1024 (128K) */
#define SLAB_SIZE 128*1024

/* blocks in freelists[i] are of size 2**i. */
#define BLOCK_SIZE(i) (1 << (i))

/* the address of the buddy of a block from freelists[i]. */
//#define BUDDY_OF(b, i) ((pointer)( ((long long int)b) ^ (1 << (i)) ))
#define _MEMBASE(base)        ((uintptr_t) base)
#define _OFFSET(base, b)      ((uintptr_t)b - _MEMBASE(base))
#define _BUDDYOF(base, b, i)  (_OFFSET(base, b) ^ (1 << (i)))
#define BUDDY_OF(base, b, i)   ((pointer)( _BUDDYOF(base, b, i) + _MEMBASE(base)))

/* used for untyped pointers */
typedef void *pointer;

/* pointers to the free space lists */
pointer freelists[BUDDY_MAX_ORDER + 1];

/* the start of managed memory */
static pointer mem_region_ptr = NULL;

typedef enum slab_color {
    EMPTY,
    PARTIAL,
    FULL
} COLOR;

struct obj_header {
    pointer block;
    unsigned is_free;
    struct obj_header *next;
};

struct slab_header {
    pointer mem_base;
    int total_objects;
    unsigned long long int bit_mask[65];    //todo: need to utilize this bit_mask to efficiently alloc free memory space
    struct obj_header *obj_head;
    //struct obj_header *obj_tail;
    //COLOR color;
    struct slab_header *next;
    struct slab_header *previous;
};

struct mem_ptr {
    int alloc_size;
    int alloc_id;
    pointer block;
};

struct slab_header *cache_list[CACHE_LIST_SIZE];
int pow_of_two[BUDDY_MAX_ORDER + 1];
static long long int global_internal_fragmentation;
static long long int global_external_fragmentation;

/* initialization */
void kmem_init();

/* malloc a continuous memory spacce */
pointer kmalloc_8144(int size);

/* free the allocated memory space */
void kfree_8144(pointer ptr);

/* purge the free slabs */
void purge_8144();

/* report the size of internal fragmentations in bytes */
int internal_frag();

/* report the size of external fragmentations (holes) in bytes */
int external_frag();

/* destructor */
void kmem_finit();

/* print buddy list */
void print_buddy();

/* print a single slab */
void print_slab(int slab_order);

#endif
