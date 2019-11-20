#include "mem_8144.h"

/* this is just sample code for debugging */
int test_kmem_id = 0;
int test_kmem_size = 0;
pthread_mutex_t test_mutex_lock;

pointer binary_buddy_allocate(int size) {
    printf("buddy called with size: %d\n", size);
    int i, order;
    pointer block, buddy;

    // calculate minimal order for this size
    i = 0;
    while (pow_of_two[i] < size) i++;

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

void print_slab(int slab_order) {
    printf("into the world of happyness ...");
    struct slab_header *current_slab = cache_list[slab_order];
    printf("slab[%d]", slab_order);

    while(current_slab != NULL) {
        struct obj_header *current_obj_chain = current_slab->obj_head;
        printf("===>{{%ld}}<==", current_obj_chain);
        while(current_obj_chain != NULL) {
            printf(" <(%d), (%ld)>", current_obj_chain->is_free, current_obj_chain->block);
            current_obj_chain = current_obj_chain->next;
        }
        if(current_slab->next == NULL) break;
        printf(" ///==>\\\\\\ ");
        current_slab = current_slab->next;
    }
    printf("\n");
}

struct slab_header *allocate_new_slab(int slab_order) {
    pointer addr = binary_buddy_allocate(SLAB_SIZE);

    printf("\t\tslab order: %d, buddy gives: %ld\n", slab_order, addr);

    if(addr == NULL) {
        perror("buddy allocation failed");
        exit(EXIT_FAILURE);
    }

    int obj_size = pow_of_two[SLAB_MIN_ORDER + slab_order];
    int obj_unit_size = sizeof(struct obj_header) + obj_size;
    int num_of_objects = (SLAB_SIZE - sizeof(struct slab_header)) / obj_unit_size;

    printf("-----------------obj size: %d, obj unit size: %d\n", obj_size, obj_unit_size);
    //int num_of_objects = (SLAB_SIZE  / obj_size);

    //todo: for now, we keep track of memory pointers in custom data-structure, later it should be updated and placed in the memory got from buddy
    struct slab_header *new_slab_header = (struct slab_header *) addr;
    new_slab_header->mem_base = addr;
    new_slab_header->total_objects = num_of_objects;
    new_slab_header->next = NULL;

    //int local_internal_fragmentation = SLAB_SIZE - sizeof(struct slab_header) - obj_unit_size * num_of_objects;
    int local_internal_fragmentation = SLAB_SIZE - sizeof(struct slab_header) - (obj_unit_size * num_of_objects);
    printf("local internal fragmentation: %d, #of objects: %d\n", local_internal_fragmentation, num_of_objects);
    global_internal_fragmentation += local_internal_fragmentation;

    //new_slab_header->obj_head = (struct obj_header *) malloc(sizeof(struct obj_header));
    addr = (pointer) ((char *) addr + sizeof(struct slab_header));
    new_slab_header->obj_head = (struct obj_header *) addr;
    struct obj_header *current_obj = new_slab_header->obj_head;
    //current_obj->block = addr;
    current_obj->block = (pointer) ((char *) addr + sizeof(struct obj_header));
    current_obj->is_free = 1;
    current_obj->next = NULL;

    addr = (pointer) ((char *) addr + obj_unit_size);
    int obj_count = 1;

    //initialize slab object chain
    for(int o=1; o<num_of_objects; o+=1) {
        //current_obj = (pointer) ((char *) addr + sizeof(struct slab_header) + (o * obj_unit_size));
        //struct obj_header *new_obj = (struct obj_header *) malloc(sizeof(struct obj_header));
        struct obj_header *new_obj = (struct obj_header *) addr;
        new_obj->block = (pointer) ((char *) addr + sizeof(struct obj_header));
        new_obj->is_free = 1;
        new_obj->next = NULL;

        current_obj->next = new_obj;
        current_obj = current_obj->next;
        obj_count += 1;

        addr = (pointer) ((char *) addr + obj_unit_size);
    }
    printf("total created object: %d\n", obj_count);
    return new_slab_header;
}

//todo: this method is working fine, trying with another way ... if failed should remove "V1" from method name
struct slab_header *allocate_new_slabV1(int slab_order) {
    pointer addr = binary_buddy_allocate(CACHE_LIST_SIZE);

    if(addr == NULL) {
        perror("buddy allocation failed");
        exit(EXIT_FAILURE);
    }

    //todo: for now, we keep track of memory pointers in custom data-structure, later it should be updated and placed in the memory got from buddy
    struct slab_header * new_slab_header = (struct slab_header *) malloc(sizeof(struct slab_header));
    //new_slab_header->color = EMPTY;

    int obj_size = pow_of_two[SLAB_MIN_ORDER + slab_order];
    //int obj_unit_size = sizeof(struct obj_header) + obj_size;
    //int num_of_objects = (SLAB_SIZE - sizeof(struct slab_header)) / obj_unit_size;
    int num_of_objects = (SLAB_SIZE  / obj_size);

    //int internal_fragmentation = SLAB_SIZE - sizeof(struct slab_header) - obj_unit_size * num_of_objects;
    int internal_fragmentation = SLAB_SIZE - (obj_size * num_of_objects);
    printf("internal fragmentation: %d, #of objects: %d\n", internal_fragmentation, num_of_objects);

    new_slab_header->obj_head = (struct obj_header *) malloc(sizeof(struct obj_header));
    struct obj_header *obj_list = new_slab_header->obj_head;
    obj_list->block = addr;
    obj_list->is_free = 1;

    //initialize slab object chain
    for(int o=1; o<num_of_objects; o+=1) {
        //obj_list = (pointer) ((char *) addr + sizeof(struct slab_header) + (o * obj_unit_size));
        struct obj_header *new_obj = (struct obj_header *) malloc(sizeof(struct obj_header));
        new_obj->block = (pointer) ((char *) addr + (o * obj_size));
        new_obj->is_free = 1;

        obj_list->next = new_obj;
        obj_list = obj_list->next;
    }
    return new_slab_header;
}

pointer get_mempointer_from_slab(int size) {
    printf("size %d requested to slab\n", size);
    int slab_order = 0;
    while(pow_of_two[slab_order] < size) slab_order += 1;
    slab_order -= SLAB_MIN_ORDER;

    struct slab_header * current_slab = cache_list[slab_order];
    while(current_slab != NULL) {
        struct obj_header * current_obj_chain = current_slab->obj_head;
        //for(int i=0; i< current_slab->total_objects; i+=1) {
        while(current_obj_chain != NULL) {
            printf("... looking for shotrues ...\n");
            if(current_obj_chain->is_free) {
                printf("found free pointer <%ld, %ld> ... sizeofheader: %ld, gap: %ld\n",
                        current_obj_chain, current_obj_chain->block,
                        sizeof(struct obj_header), ((char *) current_obj_chain->block - (char *)current_obj_chain));
                current_obj_chain->is_free = 0;
                return current_obj_chain->block;
            }
            current_obj_chain = current_obj_chain->next;
        }
        if(current_slab->next == NULL) break;
        current_slab = current_slab->next;
    }

    //don't found free slot till now, need to assign new slab
    printf("not found free pointer, trying to get new memory with order: %d\n", slab_order);
    struct slab_header *new_slab = allocate_new_slab(slab_order);
    current_slab->next = new_slab;
    current_slab = current_slab->next;
    current_slab->obj_head->is_free = 0;
    return current_slab->obj_head->block;
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

    global_external_fragmentation = 0;
    global_internal_fragmentation = 0;

    /* initialize slab caches */
    for(int i=0; i<CACHE_LIST_SIZE; i+=1) {
        cache_list[i] = allocate_new_slab(i);
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
    //pointer block = binary_buddy_allocate(size);
    //try with slab
    pointer block = get_mempointer_from_slab(size);
    ptr->block = block;
    pthread_mutex_unlock(&test_mutex_lock);

    printf("[%ld] kmalloc %d with size: %d\n", pthread_self(), local_test_id, size);
    return (pointer) ptr;
}

void kfree_8144(pointer ptr) {
    /* this is just sample code for debugging */
    struct mem_ptr *p = (struct mem_ptr *) ptr;
    pthread_mutex_lock(&test_mutex_lock);
    struct obj_header *header = (struct obj_header *) ((char *) p->block - sizeof(struct obj_header));
    header->is_free = 1;
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
    return global_internal_fragmentation;
}

int external_frag() {
    return global_external_fragmentation;
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