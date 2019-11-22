#include "mem_8144.h"

/* this is just sample code for debugging */
int test_kmem_id = 0;
int test_kmem_size = 0;

void calculate_external_fragmentation() {
    long long int local_external_fragmentation = 0;
    global_external_fragmentation = 0;
    for (int i = BUDDY_MAX_ORDER - 1; i >= 0; i -= 1) {
        if (buddy_lists[i] != NULL) {
            global_external_fragmentation += local_external_fragmentation;
            local_external_fragmentation = 0;
        } else {
            local_external_fragmentation += pow_of_two[i + BUDDY_ORDER_PADDING];
        }
    }
}

pointer binary_buddy_allocate(int size) {
    int i, order;
    pointer block, buddy;
    struct buddy_list *buddy_list_entry;

    // calculate minimal order for this size
    i = 0;
    while (pow_of_two[i] < size) i++;
    order = i = (i - BUDDY_ORDER_PADDING);

    // level up until non-null list found
    while(true) {
        if (i > BUDDY_MAX_ORDER) return NULL;
        if (buddy_lists[i] != NULL) break;
        i += 1;
    }

    // remove the block out of list
    buddy_list_entry = buddy_lists[i];
    block = buddy_list_entry->buddy_ptr;
    buddy_lists[i] = buddy_lists[i]->next;
    free(buddy_list_entry);

    // split until i == order
    while (i-- > order) {
        buddy = BUDDY_OF(mem_region_ptr, block, i + BUDDY_ORDER_PADDING);

        struct buddy_list *new_buddy_list_entry = (struct buddy_list *) malloc(sizeof(struct buddy_list));
        new_buddy_list_entry->buddy_ptr = buddy;
        new_buddy_list_entry->next = buddy_lists[i];
        buddy_lists[i] = new_buddy_list_entry;
    }

    return block;
}

void binary_buddy_deallocate(pointer block, int size) {
    int i;
    pointer buddy;
    pointer *p;

    // fetch order in previous byte
    i = 0;
    while (pow_of_two[i] < size) i++;
    i = (i - BUDDY_ORDER_PADDING);

    while (true) {
        if(i > BUDDY_MAX_ORDER) {
            return;
        }
        // calculate buddy
        buddy = BUDDY_OF(mem_region_ptr, block, i + BUDDY_ORDER_PADDING);
        struct buddy_list *current_buddy_list = buddy_lists[i];
        // find buddy in list
        while (current_buddy_list != NULL) {
            if(current_buddy_list->buddy_ptr == buddy) break;
            current_buddy_list = current_buddy_list->next;
        }

        // not found, insert into list
        if (current_buddy_list == NULL || current_buddy_list->buddy_ptr != buddy) {
            struct buddy_list *new_buddy_list_entry = (struct buddy_list *) malloc(sizeof(struct buddy_list));
            new_buddy_list_entry->buddy_ptr = block;
            new_buddy_list_entry->next = buddy_lists[i];
            buddy_lists[i] = new_buddy_list_entry;

            return;
        }

        // found, merged block starts from the lower one
        block = (block < buddy) ? block : buddy;

        // remove buddy out of list
        //todo: if this works ... will keep the previous link to do it in optimized way
        if(buddy_lists[i]->buddy_ptr == buddy) {
            struct buddy_list *tmp_buddy_entry = buddy_lists[i];
            buddy_lists[i] = buddy_lists[i]->next;
            free(tmp_buddy_entry);
        } else {
            current_buddy_list = buddy_lists[i];
            while(current_buddy_list->next->buddy_ptr != buddy) {
                current_buddy_list = current_buddy_list->next;
            }
            struct buddy_list *tmp_buddy_entry = current_buddy_list->next;
            current_buddy_list->next = current_buddy_list->next->next;
            free(tmp_buddy_entry);
        }

        i += 1;
    }
}

void print_slab(int slab_order) {
    struct slab_header *current_slab = cache_list[slab_order];
    printf("slab[%d]", slab_order);

    while (current_slab != NULL) {
        struct obj_header *current_obj_chain = current_slab->obj_head;
        while (current_obj_chain != NULL) {
            printf(" <(%d), (%p)>", current_obj_chain->is_free, current_obj_chain->block);
            current_obj_chain = current_obj_chain->next;
        }
        if (current_slab->next == NULL) break;
        printf(" ==>");
        printf("\n");
        current_slab = current_slab->next;
    }
    printf("\n");
}

/*
 * allocate_new_slab -- create and initialize new slab cache. used internal memory to hold slab information
 */
struct slab_header *allocate_new_slab(int slab_order) {
    pointer addr = binary_buddy_allocate(SLAB_SIZE);

    if (addr == NULL) {
        perror("buddy allocation failed");
        return NULL;
    }

    int obj_size = pow_of_two[SLAB_MIN_ORDER + slab_order];
    int obj_unit_size = sizeof(struct obj_header) + obj_size;
    int num_of_objects = (SLAB_SIZE - sizeof(struct slab_header)) / obj_unit_size;

    int local_internal_fragmentation = SLAB_SIZE - sizeof(struct slab_header) - (obj_unit_size * num_of_objects);
    global_internal_fragmentation += local_internal_fragmentation;

    struct slab_header *new_slab_header = (struct slab_header *) addr;
    new_slab_header->mem_base = addr;
    new_slab_header->total_objects = num_of_objects;
    new_slab_header->local_internal_fragmentation = local_internal_fragmentation;
    new_slab_header->next = NULL;

    addr = (pointer) ((char *) addr + sizeof(struct slab_header));
    new_slab_header->obj_head = (struct obj_header *) addr;
    struct obj_header *current_obj = new_slab_header->obj_head;

    current_obj->block = (pointer) ((char *) addr + sizeof(struct obj_header));
    current_obj->is_free = 1;
    current_obj->next = NULL;

    addr = (pointer) ((char *) addr + obj_unit_size);
    int obj_count = 1;

    //initialize slab object chain
    for (int o = 1; o < num_of_objects; o += 1) {
        struct obj_header *new_obj = (struct obj_header *) addr;
        new_obj->block = (pointer) ((char *) addr + sizeof(struct obj_header));
        new_obj->is_free = 1;
        new_obj->next = NULL;

        current_obj->next = new_obj;
        current_obj = current_obj->next;
        obj_count += 1;

        addr = (pointer) ((char *) addr + obj_unit_size);
    }
    return new_slab_header;
}

/*
 * allocate_new_slabV1 -- (deprecated) create and initialize new slab cache. used external memory to hold slab information
 */
struct slab_header *allocate_new_slabV1(int slab_order) {
    pointer addr = binary_buddy_allocate(CACHE_LIST_SIZE);

    if (addr == NULL) {
        perror("buddy allocation failed");
        exit(EXIT_FAILURE);
    }

    struct slab_header *new_slab_header = (struct slab_header *) malloc(sizeof(struct slab_header));

    int obj_size = pow_of_two[SLAB_MIN_ORDER + slab_order];
    int num_of_objects = (SLAB_SIZE / obj_size);

    int internal_fragmentation = SLAB_SIZE - (obj_size * num_of_objects);
    //printf("internal fragmentation: %d, #of objects: %d\n", internal_fragmentation, num_of_objects);

    new_slab_header->obj_head = (struct obj_header *) malloc(sizeof(struct obj_header));
    struct obj_header *obj_list = new_slab_header->obj_head;
    obj_list->block = addr;
    obj_list->is_free = 1;

    //initialize slab object chain
    for (int o = 1; o < num_of_objects; o += 1) {
        struct obj_header *new_obj = (struct obj_header *) malloc(sizeof(struct obj_header));
        new_obj->block = (pointer) ((char *) addr + (o * obj_size));
        new_obj->is_free = 1;

        obj_list->next = new_obj;
        obj_list = obj_list->next;
    }
    return new_slab_header;
}

pointer get_mempointer_from_slab(int size) {
    int slab_order = 0;
    while (pow_of_two[slab_order] < size) slab_order += 1;
    slab_order -= SLAB_MIN_ORDER;

    struct slab_header *current_slab = cache_list[slab_order];
    while (current_slab != NULL) {
        struct obj_header *current_obj_chain = current_slab->obj_head;
        while (current_obj_chain != NULL) {
            if (current_obj_chain->is_free) {
                current_obj_chain->is_free = 0;
                return current_obj_chain->block;
            }
            current_obj_chain = current_obj_chain->next;
        }
        if (current_slab->next == NULL) break;
        current_slab = current_slab->next;
    }

    //don't found free slot till now, need to assign new slab
    struct slab_header *new_slab = allocate_new_slab(slab_order);
    if(new_slab == NULL) {
        return NULL;
    }
    current_slab->next = new_slab;
    current_slab = current_slab->next;
    current_slab->obj_head->is_free = 0;
    return current_slab->obj_head->block;
}

void kmem_init() {
    mem_region_ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_region_ptr == NULL) {
        perror("memory init error");
        exit(EXIT_FAILURE);
    }

    printf("initial address: %p\n", mem_region_ptr);

    for (int i = 0; i <= MEM_MAX_ORDER; i += 1) pow_of_two[i] = (1 << i);

    /* initialize mutex lock */
    pthread_mutex_init(&global_mutex_lock, NULL);

    /* initialize buddy freelist at the highest order */
    for(int i=0; i<BUDDY_MAX_ORDER; i+=1) buddy_lists[i] = NULL;
    //place mem_region_ptr at the highest order in buddy_list
    struct buddy_list *buddy_list_entry = (struct buddy_list *) malloc(sizeof(struct buddy_list));
    buddy_list_entry->buddy_ptr = mem_region_ptr;
    buddy_list_entry->next = NULL;
    buddy_lists[BUDDY_MAX_ORDER] = buddy_list_entry;

    global_external_fragmentation = 0;
    global_internal_fragmentation = 0;

    /* initialize slab caches */
    for (int i = 0; i < CACHE_LIST_SIZE; i += 1) {
        cache_list[i] = allocate_new_slab(i);
    }
}

pointer kmalloc_8144(int size) {
    if (!size) return NULL;

    pthread_mutex_lock(&global_mutex_lock);
    //printf("[%s]: %ld\n", __func__, pthread_self());

    int local_test_id = 0;
    struct mem_ptr *ptr = (struct mem_ptr *) malloc(sizeof(struct mem_ptr));

    test_kmem_id += 1;
    test_kmem_size += size;
    local_test_id = test_kmem_id;
    ptr->alloc_size = size;
    ptr->alloc_id = test_kmem_id;

    //get memory pointer from slab
    pointer block = get_mempointer_from_slab(size);
    ptr->block = block;

    printf("[%ld] kmalloc with size: %d\n", pthread_self(), size);

    pthread_mutex_unlock(&global_mutex_lock);
    return (pointer) ptr;
}

void kfree_8144(pointer ptr) {
    pthread_mutex_lock(&global_mutex_lock);
    //printf("[%s]: %ld\n", __func__, pthread_self());

    struct mem_ptr *p = (struct mem_ptr *) ptr;
    if(p->block != NULL) {
        struct obj_header *header = (struct obj_header *) ((char *) p->block - sizeof(struct obj_header));
        header->is_free = 1;
        printf("[%ld]   kfree %d with size: %d\n", pthread_self(), p->alloc_id, p->alloc_size);
    }

    pthread_mutex_unlock(&global_mutex_lock);
}

bool is_empty_slab(struct slab_header *current_slab) {
    struct obj_header *current_obj_chain = current_slab->obj_head;
    while (current_obj_chain != NULL) {
        if (!current_obj_chain->is_free) return false;
        current_obj_chain = current_obj_chain->next;
    }
    return true;
}

void purge_slab_v1(struct slab_header *header) {
    while (header != NULL) {
        if (is_empty_slab(header)) {
            struct slab_header *purgeable_slab = header;
            header = header->next;
            binary_buddy_deallocate(purgeable_slab->mem_base, SLAB_SIZE);
        } else {
            header = header->next;
        }
    }
}

/* purge slab caches */
void purge_8144_v1() {
    pthread_mutex_lock(&global_mutex_lock);
    //printf("[%s]: %ld\n", __func__, pthread_self());

    for (int i = 0; i < CACHE_LIST_SIZE; i += 1) {
        purge_slab_v1(cache_list[i]);
    }
    pthread_mutex_unlock(&global_mutex_lock);
}

void purge_slab(struct slab_header *header) {
    while (header->next != NULL) {
        if (is_empty_slab(header->next)) {
            struct slab_header *purgeable_slab = header->next;
            header->next = header->next->next;
            global_internal_fragmentation -= purgeable_slab->local_internal_fragmentation;
            binary_buddy_deallocate(purgeable_slab->mem_base, SLAB_SIZE);
        } else {
            header = header->next;
        }
    }
}

/* purge slab caches */
void purge_8144() {
    pthread_mutex_lock(&global_mutex_lock);
    printf("[%s]: %ld\n", __func__, pthread_self());

    for (int i = 0; i < CACHE_LIST_SIZE; i += 1) {
        purge_slab(cache_list[i]);
    }
    pthread_mutex_unlock(&global_mutex_lock);
}

long long int internal_frag() {
    return global_internal_fragmentation;
}

long long int external_frag() {
    pthread_mutex_lock(&global_mutex_lock);
    //printf("[%s]: %ld\n", __func__, pthread_self());
    calculate_external_fragmentation();
    pthread_mutex_unlock(&global_mutex_lock);
    return global_external_fragmentation;
}

void kmem_finit() {
    pthread_mutex_destroy(&global_mutex_lock);
    global_external_fragmentation = 0;
    global_internal_fragmentation = 0;

    for (int i = 0; i < CACHE_LIST_SIZE; i += 1) {
        struct slab_header *header = cache_list[i];
        while (header != NULL) {
            struct slab_header *purgeable_slab = header;
            header = header->next;
            binary_buddy_deallocate(purgeable_slab->mem_base, SLAB_SIZE);
        }
    }

    if (mem_region_ptr != NULL) {
        int r = munmap(mem_region_ptr, MEM_SIZE);
        if (r == -1) {
            perror("memory finit error");
            exit(EXIT_FAILURE);
        }
    }
}

void print_buddy_list(int i) {
    printf("buddy_lists[%d]: \n", i);
    struct buddy_list *p = buddy_lists[i];
    printf("\t");

    while (p != NULL) {
        printf("%p ", p->buddy_ptr);
        p = p->next;
    }
    printf("\n");
}

void print_buddy() {
    int i;

    printf("========================================\n");
    printf("MEMPOOL size: %d\n", MEM_SIZE);
    printf("MEMPOOL start @ %p\n", mem_region_ptr);

    for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
        print_buddy_list(i);
    }
}