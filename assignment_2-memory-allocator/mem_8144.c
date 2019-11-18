#include "mem_8144.h"

/* this is just sample code for debugging */
int test_kmem_id = 0;
int test_kmem_size = 0;
pthread_mutex_t test_mutex_lock;

struct mem_ptr {
    int alloc_size;
    int alloc_id;
    pointer block;
};

void kmem_init() {
    /* this is just sample code for debugging */
    mem_region_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_region_ptr == NULL) {
        perror("memory init error");
        exit(EXIT_FAILURE);
    }

    printf("initial address: %ld\n", mem_region_ptr);

    //todo: initialize slabs and data structures
    pthread_mutex_init(&test_mutex_lock, NULL);
    freelists[BUDDY_MAX_ORDER] = mem_region_ptr;
    pointer buddy = BUDDY_OF(mem_region_ptr, 25);
    printf("->%ld\n", buddy);

    return;
}

pointer binary_buddy_allocate(int size) {
    int i, order;
    pointer block, buddy;

    // calculate minimal order for this size
    i = 0;
    while (BLOCK_SIZE(i) < size) // one more byte for storing order
        i++;

    order = i;// = (i < MIN_ORDER) ? MIN_ORDER : i;

    printf("size: %d, order: %d\n", size, order);

    // level up until non-null list found
    for (;; i++) {
        if (i > BUDDY_MAX_ORDER)
            return NULL;
        if (freelists[i])
            break;
    }

    // remove the block out of list
    block = freelists[i];
    printf("block: %ld, i: %d\n", block, i);
    freelists[i] = *(pointer*) freelists[i];

    // split until i == order
    while (i-- > order) {
        buddy = BUDDY_OF(block, i);
        freelists[i] = buddy;
    }

    printf("%d-> %ld\n", order, freelists[order]);
    printf("allocation order: %d\n", order);

    // store order in previous byte
    //*((uint8_t*) (block - 1)) = order;
    return block;

    /*int i;

    printf("buddy called with: %d\n", size);

    *//* compute i as the least integer such that i >= log2(size) *//*
    for (i = 0; BLOCK_SIZE(i) < size; i++);

    printf("order: %d\n", i);

    if (i > BUDDY_MAX_ORDER) {
        printf("no space available\n");
        return NULL;
    } else if (freelists[i] != NULL) {
        *//* we already have the right size block on hand *//*
        pointer block;
        block = freelists[i];
        freelists[i] = *(pointer *) freelists[i];
        return block;
    } else {
        *//* we need to split a bigger block *//*
        pointer block, buddy;
        block = binary_buddy_allocate(BLOCK_SIZE(i + 1));

        if (block != NULL) {
            printf("found space in buddy\n");
            *//* split and put extra on a free list *//*
            buddy = BUDDY_OF(block, i);
            printf("buddy: %ld\n", buddy);
            *(pointer *) buddy = freelists[i];
            printf("buddy: %ld\n", buddy);
            freelists[i] = buddy;
            printf("buddy set ... returning\n");
        }
        return block;
    }*/
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
    pointer block = binary_buddy_allocate(size);
    ptr->block = block;
    pthread_mutex_unlock(&test_mutex_lock);

    printf("[%ld] kmalloc %d with size: %d\n", pthread_self(), local_test_id, size);
    return (pointer) ptr;
}

void binary_buddy_deallocate(pointer block, int size) {
    int i;
    pointer buddy;
    pointer * p;

    // fetch order in previous byte
    //i = *((uint8_t*) (block - 1));
    i = 0;
    while (BLOCK_SIZE(i) < size) i++;

    printf("[%s] find pointer at: %d\n", __func__, i);

    for ( ; ; i++) {
        printf("~~~~~~~~~~~~~~~~~~~[%d]\n", i);
        // calculate buddy
        buddy = BUDDY_OF(block, i);
        p = &(freelists[i]);

        printf("[%s] buddy: %ld, freelist_entry: %ld\n", __func__, buddy, *p);

        // find buddy in list
        while ((*p != NULL) && (*p != buddy)) {
            p = (pointer *) *p;
        }

        printf("come here ... %ld %ld\n", *p, buddy);

        // not found, insert into list
        if (*p != buddy) {
            printf("buddy not found\n");
            *(pointer*) block = freelists[i];
            freelists[i] = block;
            return;
        }
        // found, merged block starts from the lower one
        printf("buddy found ... %ld %ld\n", block, buddy);
        block = (block < buddy) ? block : buddy;
        // remove buddy out of list
        printf("*(pointer*) *p: %ld\n", *p);
        *p = *(pointer*) *p;
        printf("done or not done...\n");
    }
}

void kfree_8144(pointer ptr) {
    /* this is just sample code for debugging */
    struct mem_ptr *p = (struct mem_ptr *) ptr;
    pthread_mutex_lock(&test_mutex_lock);
    test_kmem_size -= p->alloc_size;
    binary_buddy_deallocate(p->block, p->alloc_size);
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