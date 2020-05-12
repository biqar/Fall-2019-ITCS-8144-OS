# Fall-2019-ITCS-8144-OS
This repository contains the projects and assignments of course **"ITCS 6144/8144: Operating System Design"**. This course has been taken in Fall 2019 semester, as part of my PhD degree at UNC Charlotte.

## Assignment 1: Pthread Lock

* ITCS 6144/8144 Assignment - Pthread Lock
    * Metadata
    * Assignment Description
        * Needed Programs
        * Submissions
    * Hints
        * Pthread
        * Pthread Locks

### Metadata

| Key           | Value                                                     |
| ---           | ---                                                       |
| type          | coding assignment                                         |
| topic         | Concurrency and Lock                                      |
| release date  | Sept. 10, 2019                                            |
| deadline      | Sept. 24, 2019                                            |
| hand-in       | a zip/rar file, including README, Makefile, Codes, Report |
| platform      | Linux Only (Ubuntu)                                       |
| language      | C Only                                                    |


### Assignment Description
In this assignment, you will develop several multi-threaded pthread programs using different types of locks and compare their performance on different workloads. Detailed requirements on each of these programs are listed below.
Needed Programs

* Program 1: Mutex Lock and Spin Lock
    * develop a program using both pthread Mutex Lock and Spin Lock
    * the program takes *n* as the input parameter to denote how many threads will be created. n ∈ [2, 4, 8].
    * each thread runs 100,000 times in a loop
    * in each loop, each thread adds *1* to a shared variable (*k*) [1, 1,000, 1,000,000] times (three run cases) with lock acquired
    * for each *n*, run the program three times [1, 1,000, 1,000,000].
    * compare the execution time of using different locks for different cases and different numbers of threads.
* Program 2: Condition Variable
    * develop a program using pthread Condition Variable
    * the program takes *n* as the input parameter to denote how many threads will be created. n ∈ [1, 2, 4, 8].
    * $n-1$ threads randomly add *1* to a shared variable (*k*) in an interval of *100* ms
    * one thread wait until that shared variable reaches *100* and print something
    * no performance comparison; just show the execution time of your program
* Program 3: Reader/Writer Lock
    * develop a program using both pthread Reader/Writer Lock and Mutex Lock to implement the same logic
    * the program takes *n* as the input parameter to denote how many readers(*n*) will be created. n ∈ [2, 4, 8] and one writer
    * each of the *n* readers reads a shared variable (*k*) 10,000 times
    * the writer adds *1* to the shared variable (*k*) 10,000 times
    * compare the execution time of using different locks
    * identify and show when write happens

If you like, you can use more advanced locks (MCS and RCU) to implement the same Program 1 and compare the performance with mutex lock and spin lock. You will get extra points.

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

### Hints
#### Pthread

* A standardized multi-threaded programming interface.
* A very good pthread tutorial: https://computing.llnl.gov/tutorials/pthreads/
```
/*
 * Please compile with gcc -Wall helloworld_pthreads.c -lpthread
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS	10

void *print_hello_world(void *tid)
{
   long threadid;
   threadid = (long)tid;
   printf("Hello World! Greetings from thread #%ld!\n", threadid);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int status;
   long i;

   for(i=0; i<NUM_THREADS; i++){
     printf("Main here. Creating thread %ld\n", i);
     status = pthread_create(&threads[i], NULL, print_hello_world, (void *)i);
     if (status != 0){
       printf("ERROR; return code from pthread_create() is %d\n", status);
       exit(-1);
       }
     }
   pthread_exit(NULL);
}
```
#### Pthread Locks

* Mutex Lock
    * pthread_mutex_init
    * pthread_mutex_destroy
    * pthread_mutex_lock
    * pthread_mutex_unlock
    * pthread_mutex_trylock
* Condition Variable
    * pthread_cond_init
    * pthread_cond_destroy
    * pthread_cond_signal
    * pthread_cond_broadcast
    * pthread_cond_wait
    * pthread_cond_timedwait
* Reader/Writer Lock
    * pthread_rwlock_init
    * pthread_rwlock_destroy
    * pthread_rwlock_rdlock and pthread_rwlock_tryrdlock
    * pthread_rwlock_wrlock and pthread_rwlock_trywrlock
    * pthread_rwlock_unlock
* Spin Lock
    * pthread_spin_init
    * pthread_spin_destroy
    * pthread_spin_lock and pthread_spin_trylock
    * pthread_spin_unlock

### Optional

* Userspace RCU: https://liburcu.org
* MCS Lock: Implement your own.

## Assignment 2: Memory Allocator

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

## Course Project: Understand Linux OS via Modern Tool (Systemtap)

In building/designing/using an operating system, it is important to know its internal logic and the performance characteristics. 

In this project, you will create, justify, and apply a set of experiments (via a dynamic tracing tool, named Systemtap) to a production operating system (Linux) to characterize and understand its internal behaviors and performance characteristics. In addition, you may explore the relations between some of these quantities. In doing so, you will study how to profile/debug/optimize complex system in runtime, which could be an ultimate capability for you even in the future.

You can use, and I also suggested, to use a virtual machine (e.g., VirtualBox or VMware) to conduct this project.  

This project has three parts. First, you will need to learn what is Systemtap and what it can do; Second, you will write a number of SystemTap Scripts and some analytic programs (using C, Python, or Java) to probe various parts of the Linux Operating System to gain deep understanding about the system; Third, you will need to develop visualization tool to present nice interface to others to intuitively understand what you have collected. When you finish, you will submit your report as well as the code used to perform your experiments.

### What You Can Analyze
1. Understand OS Kernel
	* Addresses and contents of a user process’s user frame and kernel frame
2. Understand System Calls
	* Show all the system calls issued by a process or in the whole system in a given period of time
	* Trace time spent in all system calls in a per-process way
3. Understand Processes
	* Parent-Child tree of an process
	* Detailed current process information, including executable files, cmdline arguments, environment variables, its uid, gid, cpuid,  
	* Metrics: how long it takes before your user process actually starts to execute its main?
4. Understand Context Switch
	* Metrics: how often and how long task scheduling and context switch take?
5. Understand Scheduler
	* latency between a task (thread) being woken up and it actually being dispatched to a CPU
	* instruments the scheduler to track the amount of time that each process spends in running, sleeping, queuing, and waiting for io.
	* Show how often and how long process scheduling takes
6. Understand Virtual Memory System
	* trace page faults, user space frees, page ins, copy on writes and unmaps.
	* trace and analysis memory usage of a user process, identify the memory leak
7. Understand Virtual File System
	* Record file open/close/read/write/mmap operations done by process
	* Store these ops in a per-process way (together with the info of processes)
	* Trace slow file system synchronous reads and writes
8. More than this...

You also need to Store everything we have collected efficiently and Use D3J to visualize it offline
