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

    // initilize slabs and data structures
    pthread_mutex_init(&test_mutex_lock, NULL);

    return;
}

void *kmalloc_8144(int size) {
    /* this is just sample code for debugging */
    int local_test_id = 0;
    struct mem_ptr *ptr = (struct mem_ptr *) malloc(sizeof(struct mem_ptr));

    pthread_mutex_lock(&test_mutex_lock);
    test_kmem_id += 1;
    test_kmem_size += size;
    local_test_id = test_kmem_id;
    ptr->alloc_size = size;
    ptr->alloc_id = test_kmem_id;
    pthread_mutex_unlock(&test_mutex_lock);

    printf("[%d] kmalloc %d with size: %d\n", pthread_self(), local_test_id, size);
    return (void *) ptr;
}

void kfree_8144(void *ptr) {
    /* this is just sample code for debugging */
    struct mem_ptr *p = (struct mem_ptr *) ptr;
    pthread_mutex_lock(&test_mutex_lock);
    test_kmem_size -= p->alloc_size;
    pthread_mutex_unlock(&test_mutex_lock);
    printf("[%d]   kfree %d with size: %d\n", pthread_self(), p->alloc_id, p->alloc_size);
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