#include "mem_8144.h"

/* this is just sample code for debugging */
int test_kmem_id = 0;
int test_kmem_size = 0;
pthread_mutex_t test_mutex_lock;

pointer binary_buddy_allocate(int size) {
    int i, order;
    pointer block, buddy;

    // calculate minimal order for this size
    i = 0;
    while (pow_of_two[i] < size) {
        i++;
    }

    //todo: order should not be of 25, it should be of 10 ... need to remap this thing
    order = i;// = (i < MIN_ORDER) ? MIN_ORDER : i;

    //printf("size: %d, order: %d\n", size, order);

    // level up until non-null list found
    for (;; i++) {
        if (i > BUDDY_MAX_ORDER)
            return NULL;
        if (freelists[i])
            break;
    }

    // remove the block out of list
    block = freelists[i];
    freelists[i] = *(pointer *) freelists[i];

    // split until i == order
    while (i-- > order) {
        buddy = BUDDY_OF(mem_region_ptr, block, i);
        freelists[i] = buddy;
    }

    // store order in previous byte
    //*((uint8_t*) (block - 1)) = order;
    return block;
}

void binary_buddy_deallocate(pointer block, int size) {
    int i;
    pointer buddy;
    pointer *p;

    // fetch order in previous byte
    //i = *((uint8_t*) (block - 1));
    i = 0;
    while (pow_of_two[i] < size) i++;

    for (;; i++) {
        // calculate buddy
        buddy = BUDDY_OF(mem_region_ptr, block, i);
        p = &(freelists[i]);

        // find buddy in list
        while ((*p != NULL) && (*p != buddy)) {
            p = (pointer *) *p;
        }

        // not found, insert into list
        if (*p != buddy) {
            *(pointer *) block = freelists[i];
            freelists[i] = block;
            return;
        }
        // found, merged block starts from the lower one
        block = (block < buddy) ? block : buddy;
        // remove buddy out of list
        *p = *(pointer *) *p;
    }
}

void kmem_init() {
    /* this is just sample code for debugging */
    mem_region_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_region_ptr == NULL) {
        perror("memory init error");
        exit(EXIT_FAILURE);
    }

    printf("initial address: %ld\n", mem_region_ptr);

    for(int i=0; i<=BUDDY_MAX_ORDER; i+=1) pow_of_two[i] = (1 << i);

    /* initialize mutex lock */
    pthread_mutex_init(&test_mutex_lock, NULL);

    /* initialize buddy freelist at the highest order */
    freelists[BUDDY_MAX_ORDER] = mem_region_ptr;

    /* initialize slab caches */
    for(int i=0; i<CACHE_LIST_SIZE; i+=1) {
        pointer addr = binary_buddy_allocate(CACHE_LIST_SIZE);

        if(addr == NULL) {
            perror("buddy allocation failed");
            exit(EXIT_FAILURE);
        }

        //todo: for now, we keep track of memory pointers in custom data-structure, later it should be updated and placed in the memory got from buddy
        cache_list[i] = (struct slab_header *) malloc(sizeof(struct slab_header));
        cache_list[i]->color = EMPTY;

        int obj_size = pow_of_two[SLAB_MIN_ORDER + i];
        //int obj_unit_size = sizeof(struct obj_header) + obj_size;
        //int num_of_objects = (SLAB_SIZE - sizeof(struct slab_header)) / obj_unit_size;
        int num_of_objects = (SLAB_SIZE  / obj_size);

        //int internal_fragmentation = SLAB_SIZE - sizeof(struct slab_header) - obj_unit_size * num_of_objects;
        int internal_fragmentation = SLAB_SIZE - (obj_size * num_of_objects);
        printf("internal fragmentation: %d\n", internal_fragmentation);

        struct obj_header *obj_list = cache_list[i]->obj_head;

        //initialize slab object chain
        for(int o=0; o<num_of_objects; o+=1) {
            //obj_list = (pointer) ((char *) addr + sizeof(struct slab_header) + (o * obj_unit_size));
            obj_list = (struct obj_header *) malloc(sizeof(struct obj_header));
            obj_list->block = (pointer) ((char *) addr + (o * obj_size));
            obj_list->is_free = 1;
            obj_list = obj_list->next;
        }
    }

    return;
}

pointer kmalloc_8144(int size) {
    if(!size) return NULL;

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

static int count_blocks(int i) {
    int count = 0;
    pointer *p = &(freelists[i]);

    while (*p != NULL) {
        count++;
        p = (pointer*) *p;
    }
    return count;
}

static int total_free() {
    int i, bytecount = 0;

    for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
        bytecount += count_blocks(i) * BLOCK_SIZE(i);
    }
    return bytecount;
}

static void print_list(int i) {
    printf("freelists[%d]: \n", i);

    pointer *p = &freelists[i];
    printf("\t");
    while (*p != NULL) {
        printf("%ld ", *p);
        p = (pointer*) *p;
    }
    printf("\n");
}

void print_buddy() {
    int i;

    printf("========================================\n");
    printf("MEMPOOL size: %d\n", MEM_SIZE);
    printf("MEMPOOL start @ %ld\n", mem_region_ptr);
    printf("total free: %d\n", total_free());

    for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
        print_list(i);
    }
}