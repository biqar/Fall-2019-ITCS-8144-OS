#pragma warning ( disable : 4786 )

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1005
#define MAX_THREAD 10
const int THREAD_SLEEP_TIME_MS = 100 * 1000;
const int REACH_POINT = 100;

unsigned long int shared_resource;

pthread_t threads[MAX_THREAD];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct thread_data {
   int  tid, itr;
};

void *thread_safe_increment(void *arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        struct thread_data *this_thread_data = (struct thread_data *) arg;
        printf("Job#%d acquired lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        if(shared_resource == REACH_POINT) {
            pthread_cond_signal(&cond);
            printf("Job#%d releasing lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
            pthread_mutex_unlock(&mutex);
            break;
        }
        shared_resource += 1;
        printf("Job#%d releasing lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        pthread_mutex_unlock(&mutex);
        usleep(THREAD_SLEEP_TIME_MS);
    }
}

void *observer(void *arg) {
    printf("observer thread is started!\n");
    pthread_mutex_lock(&mutex);
    while(shared_resource != REACH_POINT) {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Reached to %d!\n", REACH_POINT);
    pthread_mutex_unlock(&mutex);
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
    clock_gettime(CLOCK_REALTIME, &start);

    double st=clock();
    pthread_mutex_init(&mutex, 0);
    shared_resource = 1;

    //observer thread with id #0
    struct thread_data *trd_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    trd_data->tid = 0;
    pthread_create(&threads[i], NULL, observer, (void *)trd_data);

    for(i=1; i<nt; i+=1) {
        struct thread_data *trd_data = (struct thread_data *)malloc(sizeof(struct thread_data));
        trd_data->tid = i;
        //trd_data->itr = iterators[t];
        pthread_create(&threads[i], NULL, thread_safe_increment, (void *)trd_data);
    }

    for(i=0; i<nt; i+=1) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    clock_gettime(CLOCK_REALTIME, &finish);

    printf("[Condition Variable] %d threads took: %f\n", nt, get_execution_time(start, finish));

	return 0;
}
