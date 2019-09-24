# Run Command
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
