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
//const int THREAD_LOOP = 1000;
//const int THREAD_LOOP = 100000;
//const int iterators[] = { 1, 10, 100 };
//const int iterators[] = { 1, 1000, 1000000 };

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
    //pthread_cancel(threads[1]);
    pthread_mutex_unlock(&mutex);
}

//todo: not working properly, need to check sleep part and observer awaking part
//todo: also need to know why lock_wait.c is not running
int main(int argc, char** argv) {
	//freopen("in.txt", "r", stdin);
	//freopen("out.txt", "w", stdout);

	int i = 0, j, k;
	int test = 3, t = 0, kase=0;
    int error, nt;

    if(argc > 2) {
        printf("illegal number of arguments: %d\n", argc);
        return 1;
    }

    nt = atoi(argv[1]);
//    if(nt != 2 && nt != 4 && nt != 8) {
//        printf("illegal number of threads to create: %d\n", nt);
//        return 1;
//    }

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
    //cerr << nt << " threads took: " << (clock()-st)/CLOCKS_PER_SEC << endl;
    printf("%d threads took: %f\n", nt, (clock() - st) / CLOCKS_PER_SEC);

	return 0;
}
