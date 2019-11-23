# ITCS 6144/8144 Assignment - Memory Allocator

* ITCS 6144/8144 Assignment - Memory Allocator
    * Metadata
    * Assignment Description
        * Program Structure
    * Hints
        * Get Memory
        * Initilize Slabs
        * Assumptions and Logics
        * Important Numbers
        * Find Buddy
        * Multi-threading
    * Submissions
    * References

### Metadata

| Key           | Value                                                     |
| ---           | ---                                                       |
| type          | coding assignment                                         |
| topic         | Memory Allocator                                          |
| release date  | Oct. 30, 2019                                             |
| deadline      | Nov. 19, 2019                                             |
| hand-in       | a zip/rar file, including README, Makefile, Codes, Report |
| platform      | Linux Only (Ubuntu)                                       |
| language      | C Only                                                    |


### Assignment Description
In this assignment, you will develop an emulated memory allocator using buddy algorithm and slab algorithm. Your code will be compiled together with a given test file and you are expected to run the given test program and report the performance of your allocator (including both performance and fragementation level).

#### Program Structure
We provide the basic program skeleton. To compile the program, in the decompressed project directory, please create a folder **build**, and use **cmake ..** and **make** to compile the project in the **build** folder. This will generate **mem_test** binary which is compiled with your developed libaray. Run the **mem_test** binary and report the results.

**mem_test.c** uses following APIs to interact with your library. You need to implement these APIs in **mem_8144.c** and **mem_8144.h**.
```
/* initialization */ 
void kmem_init();

/* malloc a continuous memory spacce */ 
void *kmalloc_8144(int size); 

/* free the allocated memory space */ 
void kfree_8144(void *ptr); 

/* purge the free slabs */ 
void purge_8144(); 

/* report the size of internal fragementations in bytes */ 
int internal_frag(); 

/* report the size of external fragementations (holes) in bytes */ 
int external_frag();

/* destructor */
void kmem_finit();
```
### Hints

#### Get Memory
First of all, you should not use any libc memory allocation functions. The only system function you can use for memory is mmap().

mmap() function will create a mapping in the virtual meory of the current process. The address space consist of multiple pages and each page can be mapped some resource. You can create a continuous virutal memory space (32MB) for this assignment.
```
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags , int fd, off_t offset)
```
* **void \*addr** is the address we want to start mapping, typically NULL
* **size_t** length is the size we want to map in as integer
* **PROT_READ | PROT_WRITE | PROT_EXEC** options about page
* **MAP_ANONYMOUS | MAP_PRIVATE** options about page
* **MAP_ANONYMOUS** anonymous mapping without a file id attached. Here fd can be -1
    * **MAP_PRIVATE** will map given page and this will not visible to other processes.
    * **MAP_SHARED** will map given page and this will be also visible by other processes.

In this assignment, you might want to use this function to get memory:
```
mmap(NULL, MEM_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
```
Once get the initial memory pages, your library should manage them using the buddy and slab algorithms.

#### Initilize Slabs
The slab algorithm should initilize memory caches for objects in different sizes. The smallest object size is 2^5 (32) and the maximal size is 2^14 (16384). All slabs have the same size 128 * 1024 (128K), which means the slab cache for 32-byte objects contains 4096 objects.

For each size, please at least keep one slab even there is no object in the slab cache. purge() only frees memory to buddy algorithm when extra slab was created.

#### Assumptions and Logics
1. **mem_test.c** calls **kmem_init()** and **kmem_finit()** to initilize and finilize the memory regions as well as the necessary data structures.

2. **mem_test.c** calls **kmalloc_8144()** to allocate a given size memory. This memory should come from the slab area only. When more pages are needed for the slab area, you should call buddy allocator to allocate more page frames for the slab area.

3. **mem_test.c** calls **kfree_8144()** to free the allocated memory. The freed memory should be returned back to slab system. The slab system should return the free pages to buddy allocator when **purge_8144()** is called

4. **mem_test.c** calls **internal_frag()** and **external_frag()** to calculate the internal fragementation and external fragementation of your allocator.

#### Important Numbers
In **mem_test.c**, we define several constants to control the behaviors of the test program.

* NUM_THREADS: the number of threads that run the **mem_ops()** function
* NUM_ALLOC_OPS: the number of memory allocation before free them
* NUM_REPEAT: control how many times we need to repeat the loop.

We currently expect your program is able to run with **NUM_THREADS=4**; **NUM_ALLOC_OPS=1024**; **NUM_REPEAT=8**.

There are also number of constants defined in mem_8144.h.

#### Find Buddy
Given the address of an allocated block, we can easily find its buddy by XOR its address. Suppose we have block B1 of order O, we can compute the buddy as follow:
```
B2 = B1 XOR (1 << O)
```

#### Multi-threading
Make sure your code works in multi-thread applicacitons (i.e., use locks to protect your data structures)

### Submissions
The submitted package should include:
1. a README file describing how to compile and run your code;
2. a Makefile to help me easily repeat your program
    * since we do not have TA, I will not be responsible for learning how to run your program;
3. your Source Code;
4. a Report describing
    1. your test platform
    2. your results (better with plots)
    3. explain why you get the results

### References

* https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory

### Run Command
```
-- go to project directory
$ cd path-to-directory/assignment_2-memory-allocator/

-- create build directory
$ mkdir build

-- go to build directory
$ cd build

-- create make file from CMakeLists, make file will be generated in the same "build" directory
$ cmake ..

-- make the project, executable files will be generated in the same "build" directory
$ make clean && make

-- run the program
$ ./mem_test
```
