#pragma warning ( disable : 4786 )

#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

const int inf = (1<<28);
const int MAX = 1005;
const int MAX_THREAD = 10;
const int THREAD_SLEEP_TIME_MS = 100 * 1000;
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
    while(true) {
        struct thread_data *this_thread_data = (struct thread_data *) arg;
        printf("Job#%d has started\n", this_thread_data->tid);

        pthread_mutex_lock(&mutex);
        printf("Job#%d acquired lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        shared_resource += 1;
        if(shared_resource % 10 == 0) {
            printf("signalling to the observer ...\n");
            pthread_cond_signal(&cond);
        }
        //usleep(THREAD_SLEEP_TIME_MS);
        sleep(1000000);
        printf("Job#%d releasing lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        pthread_mutex_unlock(&mutex);

        printf("Job#%d has finished\n", this_thread_data->tid);
    }
}

void *observer(void *arg) {
    printf("observer is called ...\n");
    while(true) {
        pthread_mutex_lock(&mutex);
        if(shared_resource % 10 == 0) {
            printf("Yeah .... Got it!\n");
            pthread_cancel(threads[1]);
            break;
        } else {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
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
    if(nt != 2 && nt != 4 && nt != 8) {
        printf("illegal number of threads to create: %d\n", nt);
        return 1;
    }

    double st=clock();
    pthread_mutex_init(&mutex, 0);
    shared_resource = 1;

    for(i=1; i<nt; i+=1) {
        struct thread_data *trd_data = (struct thread_data *)malloc(sizeof(struct thread_data));
        trd_data->tid = i;
        //trd_data->itr = iterators[t];
        pthread_create(&threads[i], NULL, thread_safe_increment, (void *)trd_data);
    }

    //observer thread with id #0
    struct thread_data *trd_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    trd_data->tid = 0;
    pthread_create(&threads[i], NULL, observer, (void *)trd_data);

    for(i=0; i<nt; i+=1) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    //printf("~~~~~~~~~~~~~done with %d iteration~~~~~~~~~~~~~\n", iterators[t]);
    cerr << nt << " threads took: " << (clock()-st)/CLOCKS_PER_SEC << endl;

	return 0;
}
