#include "mem_8144.h"

/* this is just sample code for debugging */
int test_kmem_id = 0;
int test_kmem_size = 0;
pthread_mutex_t test_mutex_lock;

struct mem_ptr {
    int alloc_size;
    int alloc_id;
};

void kmem_init() {
    /* this is just sample code for debugging */
    mem_region_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_region_ptr == NULL) {
        perror("memory init error");
        exit(EXIT_FAILURE);
    }

    //todo: initialize slabs and data structures
    pthread_mutex_init(&test_mutex_lock, NULL);

    return;
}

pointer binary_buddy_allocate(int size) {
    int i;

    /* compute i as the least integer such that i >= log2(size) */
    for (i = 0; BLOCK_SIZE(i) < size; i++);

    if (i >= BUDDY_MAX_ORDER) {
        printf("no space available\n");
        return NULL;
    } else if (freelists[i] != NULL) {
        /* we already have the right size block on hand */
        pointer block;
        block = freelists[i];
        freelists[i] = *(pointer *) freelists[i];
        return block;
    } else {
        /* we need to split a bigger block */
        pointer block, buddy;
        block = binary_buddy_allocate(BLOCK_SIZE(i + 1));

        if (block != NULL) {
            printf("found space in buddy\n");
            /* split and put extra on a free list */
            buddy = BUDDY_OF(block, i);
            *(pointer *) buddy = freelists[i];
            freelists[i] = buddy;
        }
        return block;
    }
}

pointer kmalloc_8144(int size) {
    /* this is just sample code for debugging */
    int local_test_id = 0;
    struct mem_ptr *ptr = (struct mem_ptr *) malloc(sizeof(struct mem_ptr));

    pthread_mutex_lock(&test_mutex_lock);
    //todo: allocation logic will go here
    test_kmem_id += 1;
    test_kmem_size += size;
    local_test_id = test_kmem_id;
    ptr->alloc_size = size;
    ptr->alloc_id = test_kmem_id;

    //try with buddy
    binary_buddy_allocate(size);
    pthread_mutex_unlock(&test_mutex_lock);

    printf("[%ld] kmalloc %d with size: %d\n", pthread_self(), local_test_id, size);
    return (pointer) ptr;
}

void kfree_8144(pointer ptr) {
    /* this is just sample code for debugging */
    struct mem_ptr *p = (struct mem_ptr *) ptr;
    pthread_mutex_lock(&test_mutex_lock);
    test_kmem_size -= p->alloc_size;
    pthread_mutex_unlock(&test_mutex_lock);
    printf("[%ld]   kfree %d with size: %d\n", pthread_self(), p->alloc_id, p->alloc_size);
    return;
}

void purge_8144() {
    /* this is just sample code for debugging */
    printf("  purge memory, system used memory: %d\n", test_kmem_size);
    return;
}

int internal_frag() {
    /* this is just sample code for debugging */
    return test_kmem_size;
}

int external_frag() {
    /* this is just sample code for debugging */
    return test_kmem_size;
}

void kmem_finit() {
    /* this is just sample code for debugging */
    if (mem_region_ptr != NULL) {
        int r = munmap(mem_region_ptr, MEM_SIZE);
        if (r == -1) {
            perror("memory finit error");
            exit(EXIT_FAILURE);
        }
    }
    return;
}