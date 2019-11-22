#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "mem_8144.h"

#define NUM_THREADS 8
#define NUM_ALLOC_OPS 256
#define NUM_REPEAT 8

long microsec(struct timeval t) {
    return ((t.tv_sec * 1000000 + t.tv_usec));
}

pointer mem_ops(pointer vargp) {
    int thread_id = *(int *) vargp;
    pointer allocated_mem_ptr[NUM_ALLOC_OPS];
    int size[SLAB_MAX_ORDER + 1];
    srand(time(NULL));

    for (int i = 0; i <= SLAB_MAX_ORDER; i++) {
        size[i] = 1 << i;
    }

    /* This section allocates and frees all the memory */
    for (int k = 1; k <= NUM_REPEAT; k++) {
        /* each time, we allocate set of memory and free them */
        for (int i = 0; i < NUM_ALLOC_OPS; i++) {
            int idx = rand() % (SLAB_MAX_ORDER - SLAB_MIN_ORDER + 1) + SLAB_MIN_ORDER;
            allocated_mem_ptr[i] = kmalloc_8144(size[idx] - thread_id);
        }
        for (int i = 0; i < NUM_ALLOC_OPS; i++) {
            kfree_8144(allocated_mem_ptr[i]);
        }

        /* pick and try to purge free slabs*/
        if (k % 2 == 0) {
            purge_8144();
        }
    }

    /* This section will do arbitrary memory allocation.
     * No free. kmem_finit() should free all the data structures*/
    for (int k = 1; k < NUM_REPEAT; k++) {
        for (int i = 0; i < NUM_ALLOC_OPS; i++) {
            int idx = rand() % (SLAB_MAX_ORDER - SLAB_MIN_ORDER + 1) + SLAB_MIN_ORDER;
            for (int j = 0; j < (SLAB_MAX_ORDER - idx); j++) {
                kmalloc_8144(size[idx] + thread_id);
            }
        }
    }
    return NULL;
}

void test_memory_allocator_concurrent() {
    struct timeval tic, toc;
    long time_taken = 0L;

    pthread_t threads[NUM_THREADS];
    int status, i;

    /* initilize memory allocator */
    kmem_init();

    gettimeofday(&tic, NULL);
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&threads[i], NULL, mem_ops, (pointer) &i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&toc, NULL);
    time_taken = (microsec(toc) - microsec(tic));

    printf("ops finish in %ld us\n", time_taken);
    printf("result internal fragementations: %lld bytes\n", internal_frag());
    printf("result external fragementations: %lld bytes\n", external_frag());

    /* free allocated memory */
    //kmem_finit();
}

/* test correctness of the slab memory allocator implementation
 * so in this test what we have done,
 *      - called kmalloc_8144() in a way so that we will have three connected slab caches for a specific size
 *      - called kfree_8144() for the middle slab cache
 *      - called purge_8144() and observe the integrity of slab caches
 * slab implementation is correct!
 * */
void test_memory_allocator_single_thread_1() {
    /* initilize memory allocator */
    kmem_init();

    pointer allocated_mem_ptr[20];
    for (int i = 0; i < 15; i += 1) allocated_mem_ptr[i] = kmalloc_8144(pow_of_two[14]);
    print_slab(9);
    for (int i = 7; i < 14; i += 1) kfree_8144(allocated_mem_ptr[i]);

    print_slab(9);
    purge_8144();
    print_slab(9);
    print_buddy();

    printf("result internal fragementations: %lld bytes\n", internal_frag());
    printf("result external fragementations: %lld bytes\n", external_frag());

    /* free allocated memory */
    kmem_finit();
}

/* test correctness of the buddy implementation. test case is designed for the following specification,
 * total memory: 4 MB (i.e. 2^25 Bytes), each slab cache will hold memory: 128 KB (i.e. 2^17 Bytes)
 * so, we will have at best 256 (i.e. 2^8) slab caches
 * initially, 10 slab caches is initialized
 *
 * if the object size is 2^14, we will able to allocate 7 objects in each slab cache
 * for 2^14 size objects, we can occupy at best (256-10+1) slab caches (i.e. 9 caches is occupied for objects of other sizes)
 *
 * for this test case, we call a special purge method purge_8144_v1()
 * in the purge_8144() method, we always purge by keeping a single slab cache for each size of objects
 * but in this purge method, we purge a slab cache if it is empty and return it back to buddy
 *
 * so in this test what we have done,
 *      - initialize the slabs
 *      - occupy maximum possible slab caches
 *      - free all the occupied memory
 *      - purge all the free slabs
 *      - and we get the single unit of buddy back
 * buddy implementation is correct!
 * */
void test_memory_allocator_single_thread_2() {
    pointer allocated_mem_ptr[7 * 256];

    /* initilize memory allocator */
    kmem_init();

    for (int i = 0; i < 7 * 247; i += 1) {
        allocated_mem_ptr[i] = kmalloc_8144(pow_of_two[14]);
        printf("[%d] mem: %p\n", i, ((struct mem_ptr *) allocated_mem_ptr[i])->block);
    }
    for (int i = 0; i < 7 * 247; i += 1) {
        printf("[%d] mem: %p\n", i, ((struct mem_ptr *) allocated_mem_ptr[i])->block);
        kfree_8144(allocated_mem_ptr[i]);
    }

    purge_8144_v1();
    print_buddy();

    /* free allocated memory */
    kmem_finit();
}

int main() {
    //test_memory_allocator_single_thread_1();
    //test_memory_allocator_single_thread_2();
    test_memory_allocator_concurrent();

    return 0;
}
