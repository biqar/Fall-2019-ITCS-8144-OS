#ifndef _MEM_8144_H
#define _MEM_8144_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

/* Total size of the memory region */
#define MEM_SIZE 32*1024*1024

/* maximal order of buddy algorithm */
#define BUDDY_MAX_ORDER 25

/* minimal object size of a slab area: 32 bytes */
#define SLAB_MIN_ORDER 5

/* maximal object size of a slab area: 2^14 bytes */
#define SLAB_MAX_ORDER 14

/* blocks in freelists[i] are of size 2**i. */
#define BLOCK_SIZE(i) (1 << (i))

/* the address of the buddy of a block from freelists[i]. */
#define BUDDY_OF(b, i) ((pointer)( ((long long int)b) ^ (1 << (i)) ))

/* used for untyped pointers */
typedef void * pointer;

/* pointers to the free space lists */
pointer freelists[BUDDY_MAX_ORDER + 1];

/* the start of managed memory */
static pointer mem_region_ptr = NULL;

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

#endif
