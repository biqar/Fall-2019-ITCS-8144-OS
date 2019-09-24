CC=gcc
CFLAGS=-I.
directory=build

all:
	mkdir -p $(directory)
	$(CC) $(CFLAGS) p1_mutex_lock.c -o $(directory)/p1_mutex_lock -pthread
	$(CC) $(CFLAGS) p1_spin_lock.c -o $(directory)/p1_spin_lock -pthread
	$(CC) $(CFLAGS) p2_condition_variable.c -o $(directory)/p2_condition_variable -pthread
	$(CC) $(CFLAGS) p3_rw_lock.c -o $(directory)/p3_rw_lock -pthread
	$(CC) $(CFLAGS) p3_rw_lock_mutex.c -o $(directory)/p3_rw_lock_mutex -pthread
