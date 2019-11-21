#include "mem_8144.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

//todo: update the following values according to the problem description
#define NUM_THREADS 1
#define NUM_ALLOC_OPS 6
#define NUM_REPEAT 1

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
        if (k % 10 == 0) {
            purge_8144();
        }
    }

    print_buddy();

    /* This section will do arbitrary memory allocation.
     * No free. kmem_finit() should free all the data structures*/
    for (int k = 1; k < NUM_REPEAT; k++) {
        for (int i = 0; i < NUM_ALLOC_OPS; i++) {
            int idx = rand() % (SLAB_MAX_ORDER - SLAB_MIN_ORDER + 1) + SLAB_MIN_ORDER;
            for (int i = 0; i < (SLAB_MAX_ORDER - idx); i++)
                kmalloc_8144(size[idx] + thread_id);
        }
    }
    return NULL;
}

int main() {
    struct timeval tic, toc;
    long time_taken = 0L;

    pthread_t threads[NUM_THREADS];
    int status, i;

    /* initilize memory allocator */
    kmem_init();

    //todo: after manual testing, will uncomment the following block
    /*gettimeofday(&tic, NULL);
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&threads[i], NULL, mem_ops, (pointer) &i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&toc, NULL);
    time_taken = (microsec(toc) - microsec(tic));

    printf("ops finish in %ld us\n", time_taken);
    printf("result internal fragementations: %d bytes\n", internal_frag());
    printf("result external fragementations: %d bytes\n", external_frag());*/

    pointer allocated_mem_ptr[NUM_ALLOC_OPS];
    for (int i = 0; i < 15; i += 1) allocated_mem_ptr[i] = kmalloc_8144(pow_of_two[14]);
    printf("insert done ... ekhon ki hobe france?\n");
    print_slab(9);
    for (int i = 7; i < 14; i += 1) kfree_8144(allocated_mem_ptr[i]);

    print_slab(9);
    purge_8144();
    print_slab(9);
    print_buddy();

    printf("internal fragmentation: %ld Bytes\n", internal_frag());
    printf("external fragmentation: %ld Bytes\n", external_frag());

    /* free allocated memory */
    kmem_finit();

    exit(0);
}
