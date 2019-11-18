#ifndef _MEM_8144_H
#define _MEM_8144_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

#define MEM_SIZE 32*1024*1024   /* Total size of the memory region */
#define BUDDY_MAX_ORDER 10      /* maximal order of buddy algorithm */
#define SLAB_MIN_ORDER 5        /* minimal object size of a slab area: 32 bytes */
#define SLAB_MAX_ORDER 14       /* maximal object size of a slab area: 2^14 bytes */

static void *mem_region_ptr = NULL;    /* the start of managed memory */

void kmem_init();

void kmem_finit();

void *kmalloc_8144(int size);

void kfree_8144(void *ptr);

void purge_8144();

int internal_frag();

int external_frag();

#endif
