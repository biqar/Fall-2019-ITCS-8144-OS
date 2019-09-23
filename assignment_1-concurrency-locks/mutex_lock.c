#pragma warning ( disable : 4786 )

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 1005
#define MAX_THREAD 10
#define THREAD_LOOP 1000
//const int THREAD_LOOP = 100000;
const int iterators[] = { 1, 10, 100 };
//const int iterators[] = { 1, 1000, 1000000 };

unsigned long int shared_resource;

pthread_t threads[MAX_THREAD];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_data {
    int  tid, itr;
};

void *thread_safe_increment(void *arg) {
    struct thread_data *this_thread_data = (struct thread_data *) arg;
    printf("Job#%d has started\n", this_thread_data->tid);

    for(int i=0; i<THREAD_LOOP; i+=1) {
        pthread_mutex_lock(&mutex);
        printf("Job#%d acquired lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        for(int j=0; j<this_thread_data->itr; j+=1) {
            shared_resource += 1;
        }
        printf("Job#%d releasing lock with shared_resource value: %lu\n", this_thread_data->tid, shared_resource);
        pthread_mutex_unlock(&mutex);
    }

    printf("Job#%d has finished\n", this_thread_data->tid);

}

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

    for(t=0; t<test; t+=1) {
        double st=clock();
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

        //todo: need to get a replace cerr for C
        printf("%d threads with iteration %d took: %f\n", nt, iterators[t], (clock() - st) / CLOCKS_PER_SEC);
    }

	return 0;
}
