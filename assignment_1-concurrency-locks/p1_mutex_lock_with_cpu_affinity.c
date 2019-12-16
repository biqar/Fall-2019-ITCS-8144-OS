#pragma warning ( disable : 4786 )

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 1005
#define MAX_THREAD 10
//#define THREAD_LOOP 1000
#define THREAD_LOOP 100000
//const int iterators[] = { 1, 10, 100 };
const int iterators[] = { 1, 1000, 1000000 };

unsigned long int shared_resource;

pthread_t threads[MAX_THREAD];
pthread_mutex_t mutex;

struct thread_data {
    int  tid, itr;
};

void *thread_safe_increment(void *arg) {
    struct thread_data *this_thread_data = (struct thread_data *) arg;

    // Add cpu affinity here:
    // Create a cpu_set_t object representing a set of CPUs. Clear it and mark only CPU "this_thread_data->tid" as set
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(this_thread_data->tid, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        printf("Error calling pthread_setaffinity_np: %d\n", rc);
        exit(0);
    }

    //printf("Job#%d has started\n", this_thread_data->tid);

    for(int i=0; i<THREAD_LOOP; i+=1) {
        pthread_mutex_lock(&mutex);
        //printf("thread %ld: on cpu %d\n", pthread_self(), sched_getcpu());
        //printf("Job#%d acquired lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        for(int j=0; j<this_thread_data->itr; j+=1) {
            shared_resource += 1;
        }
        //printf("Job#%d releasing lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        pthread_mutex_unlock(&mutex);
    }

    //printf("Job#%d has finished\n", this_thread_data->tid);
}

double get_execution_time(struct timespec start, struct timespec finish) {
    long seconds = finish.tv_sec - start.tv_sec;
    long ns = finish.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > finish.tv_nsec) { // clock underflow
        --seconds;
        ns += 1000000000;
    }
    return (double)seconds + (double)ns/(double)1000000000;
}

int main(int argc, char** argv) {
	//freopen("in.txt", "r", stdin);
	//freopen("out.txt", "w", stdout);

	int i = 0, j, k;
	int test = 3, t = 0, kase=0;
    int error, nt;
    struct timespec start, finish;

    if(argc > 2) {
        printf("illegal number of arguments: %d\n", argc);
        return 1;
    }

    nt = atoi(argv[1]);

    for(t=0; t<test; t+=1) {
        clock_gettime(CLOCK_REALTIME, &start);
        pthread_mutex_init(&mutex, NULL);
        shared_resource = 0;
        for(i=0; i<nt; i+=1) {
            struct thread_data *trd_data = (struct thread_data *) malloc(sizeof(struct thread_data));
            trd_data->tid = i;
            trd_data->itr = iterators[t];

            pthread_create(&threads[i], NULL, thread_safe_increment, (void *)trd_data);
        }

        for(i=0; i<nt; i+=1) {
            pthread_join(threads[i], NULL);
        }
        pthread_mutex_destroy(&mutex);
        clock_gettime(CLOCK_REALTIME, &finish);

        printf("[Mutex Lock] %d threads with iteration %d took: %f\n", nt, iterators[t], get_execution_time(start, finish));
    }

	return 0;
}
