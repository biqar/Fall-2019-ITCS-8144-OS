# ITCS 6144/8144 Assignment - Pthread Lock

### Assignment Objective
In this assignment, I have developed several multi-threaded pthread programs using different types of locks and compare their performance on different workloads. Detailed requirements on each of these programs are listed below -

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

### Program 1: Mutex Lock and Spin Lock
For this problem, I prepared two programs using both pthread Mutex Lock and Spin Lock. The execution time is showing bellow:
```
$ ./build/p1_mutex_lock 2
[Mutex Lock] 2 threads with iteration 1 took: 0.020013
[Mutex Lock] 2 threads with iteration 1000 took: 1.009162
[Mutex Lock] 2 threads with iteration 1000000 took: 820.893171

$ ./build/p1_mutex_lock 4
[Mutex Lock] 4 threads with iteration 1 took: 0.056631
[Mutex Lock] 4 threads with iteration 1000 took: 1.940355
[Mutex Lock] 4 threads with iteration 1000000 took: 1855.090460

$ ./build/p1_mutex_lock 8
[Mutex Lock] 8 threads with iteration 1 took: 0.158721
[Mutex Lock] 8 threads with iteration 1000 took: 3.768429
[Mutex Lock] 8 threads with iteration 1000000 took: 3687.262856
```

<table>
  <tr>
    <td>
       <img align="left" src="https://github.com/biqar/Fall-2019-ITCS-8144-OS/blob/master/assignment_1-concurrency-locks/report/prog_1_compare_a.png" alt="mutex_vs_spin_compare_it_1" title="Figure 1.1: # of Iterations 1"/>
    </td>
    <td><img align="left" src="https://github.com/biqar/Fall-2019-ITCS-8144-OS/blob/master/assignment_1-concurrency-locks/report/prog_1_compare_b.png" alt="mutex_vs_spin_compare_it_1000" title="Figure 1.2: # of Iterations 1,000"/>
    </td>
    <td><img align="left" src="https://github.com/biqar/Fall-2019-ITCS-8144-OS/blob/master/assignment_1-concurrency-locks/report/prog_1_compare_c.png" alt="mutex_vs_spin_compare_it_1000000" title="Figure 1.3: # of Iterations 1,000,000"/>
    </td>
  </tr>
  <tr>
    <td align="middle">Figure 1.1: # of Iterations 1</td>
    <td align="middle">Figure 1.2: # of Iterations 1,000</td>
     <td align="middle">Figure 1.3: # of Iterations 1,000,000</td>
  </tr>
  <tr>
    <td colspan="3" align="middle">Figure 1: Mutex Lock Vs. Spin Lock: Varying Iteration Count</td>
  </tr>
</table>

```
$ ./build/p1_spin_lock 2
[Spin Lock] 2 threads with iteration 1 took: 0.011201
[Spin Lock] 2 threads with iteration 1000 took: 0.683132
[Spin Lock] 2 threads with iteration 1000000 took: 659.433738

$ ./build/p1_spin_lock 4
[Spin Lock] 4 threads with iteration 1 took: 0.034115
[Spin Lock] 4 threads with iteration 1000 took: 1.475001
[Spin Lock] 4 threads with iteration 1000000 took: 1336.079932

$ ./build/p1_spin_lock 8
[Spin Lock] 8 threads with iteration 1 took: 0.083648
[Spin Lock] 8 threads with iteration 1000 took: 2.620485
[Spin Lock] 8 threads with iteration 1000000 took: 2546.134466
```

### Program 2: Condition Variable
For this problem, I develop a program using pthread Condition Variable. The program takes n as the input parameter to denote how many threads will be created. n will be in range of [2, 4, 8]. n-1 threads randomly add 1 to a shared variable k in an interval of 100 ms. One thread wait until that shared variable reaches 100 and print “Reached to 100!”.

### Program 3: Reader/Writer Lock
For this problem, I prepared two programs using both pthread Reader Writer Lock and Mutex Lock. The execution time is showing bellow:
```
$ ./build/p3_rw_lock 2
[Reader/Writer Lock] 2 reader threads took: 0.019652

$ ./build/p3_rw_lock 4
[Reader/Writer Lock] 4 reader threads took: 0.016154

$ ./build/p3_rw_lock 8
[Reader/Writer Lock] 8 reader threads took: 0.020346
```

<table>
  <tr>
    <td>
       <img align="left" src="https://github.com/biqar/Fall-2019-ITCS-8144-OS/blob/master/assignment_1-concurrency-locks/report/prog_3_compare.png" alt="rw_vs_mutex_compare" title="Figure 2: R/W Lock Vs. Mutex Lock: Varying # of Threads"/>
    </td>
  </tr>
  <tr>
    <td align="middle">Figure 2: R/W Lock Vs. Mutex Lock: Varying # of Threads</td>
   </tr>
</table>

```
$ ./build/p3_rw_lock_mutex 2
[R/W Mutex Lock] 2 reader thread took: 0.000484

$ ./build/p3_rw_lock_mutex 4
[R/W Mutex Lock] 4 reader thread took: 0.000534

$ ./build/p3_rw_lock_mutex 8
[R/W Mutex Lock] 8 reader thread took: 0.000756
```

**Note:** As I have noticed a huge fluctuation in the execution time, I run each of the programs 100 times and then take the average.

### Result analysis
In this implementation, mutex lock can have only one reader or writer at a time. On the other hand rw lock can have one writer or multiple reader at a time. But still the mutex lock outperform rw lock. 

This is because, rw lock requires its contents to be Sync and mutex lock only requires Send. In rw lock, multiple threads may access the shared resource simultaneously. But in mutex lock, it sends data to the thread who owns the lock and after unlocking it, it will be send to another thread.

### Future Work
* Should try to implement more advanced locks (MCS and RCU) to implement the same Program 1 and compare the performance with mutex lock and spin lock.

### Run Command
```
-- make the project, executable files will be stored in "build" directory
$ make

-- Program 1# run mutex lock program: $ ./build/p1_mutex_lock {NUMBER_OF_THREAD}
$ ./build/p1_mutex_lock 2
$ ./build/p1_mutex_lock 4
$ ./build/p1_mutex_lock 8

-- Program 1# run spin lock program: $ ./build/p1_spin_lock {NUMBER_OF_THREAD}
$ ./build/p1_spin_lock 2
$ ./build/p1_spin_lock 4
$ ./build/p1_spin_lock 8

-- Program 2# run conditional variable program: $ ./build/p2_condition_variable {NUMBER_OF_THREAD}
$ ./build/p2_condition_variable 2
$ ./build/p2_condition_variable 4
$ ./build/p2_condition_variable 8

-- Program 3# run r/w lock program: $ ./build/p3_rw_lock {NUMBER_OF_THREAD}
$ ./build/p3_rw_lock 2
$ ./build/p3_rw_lock 4
$ ./build/p3_rw_lock 8

-- Program 3# run r/w lock using mutex lock program: $ ./build/p3_rw_lock_mutex {NUMBER_OF_THREAD}
$ ./build/p3_rw_lock_mutex 2
$ ./build/p3_rw_lock_mutex 4
$ ./build/p3_rw_lock_mutex 8
```
