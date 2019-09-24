#pragma warning ( disable : 4786 )

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1005
#define MAX_THREAD 100
//const int THREAD_LOOP = 10;
const int THREAD_LOOP = 10000;

struct thread_data {
    int tid, itr;
};

unsigned long int shared_resource;

pthread_t readers[MAX_THREAD], writer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *reader_thread_op(void *arg) {
    for (int i = 0; i < THREAD_LOOP; i += 1) {
        pthread_mutex_lock(&mutex);
        unsigned long int current = shared_resource;
        //printf("reader thread #%d: %lu\n", ((struct thread_data *) arg)->tid, shared_resource);
        pthread_mutex_unlock(&mutex);
    }
}

void *writer_thread_op(void *arg) {
    for (int i = 0; i < THREAD_LOOP; i += 1) {
        pthread_mutex_lock(&mutex);
        shared_resource += 1;
        printf("writer thread #0: %lu\n", shared_resource);
        pthread_mutex_unlock(&mutex);
    }
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

int main(int argc, char **argv) {
    //freopen("in.txt", "r", stdin);
    //freopen("out.txt", "w", stdout);

    int i = 0, j, k;
    int test = 3, t = 0, kase = 0;
    int error, nt;
    struct timespec start, finish;

    if (argc > 2) {
        printf("illegal number of arguments: %d\n", argc);
        return 1;
    }

    nt = atoi(argv[1]);

    clock_gettime(CLOCK_REALTIME, &start);
    shared_resource = 0;

    //writer thread
    struct thread_data *trd_data = (struct thread_data *) malloc(sizeof(struct thread_data));
    trd_data->tid = nt;
    pthread_create(&writer, NULL, writer_thread_op, (void *) trd_data);

    //reader threads
    for (i = 0; i < nt; i += 1) {
        struct thread_data *trd_data = (struct thread_data *) malloc(sizeof(struct thread_data));
        trd_data->tid = i;
        pthread_create(&readers[i], NULL, reader_thread_op, (void *) trd_data);
    }

    pthread_join(writer, NULL);
    for (i = 0; i < nt; i += 1) {
        pthread_join(readers[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    clock_gettime(CLOCK_REALTIME, &finish);

    printf("[R/W Mutex Lock] %d reader thread took: %f\n", nt, get_execution_time(start, finish));

    return 0;
}
