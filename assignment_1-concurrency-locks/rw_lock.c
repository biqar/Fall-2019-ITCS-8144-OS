#pragma warning ( disable : 4786 )

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1005
#define MAX_THREAD 10
//const int THREAD_LOOP = 10;
const int THREAD_LOOP = 10000;

struct thread_data {
    int tid, itr;
};

unsigned long int shared_resource;

pthread_t readers[MAX_THREAD], writer;
pthread_rwlock_t rwlock;

void *reader_thread_op(void *arg) {
    for (int i = 0; i < THREAD_LOOP; i += 1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("reader thread #%d: %lu\n", ((struct thread_data *) arg)->tid, shared_resource);
        pthread_rwlock_unlock(&rwlock);
    }
}

void *writer_thread_op(void *arg) {
    for (int i = 0; i < THREAD_LOOP; i += 1) {
        pthread_rwlock_wrlock(&rwlock);
        shared_resource += 1;
        printf("writer thread #0: %lu\n", shared_resource);
        pthread_rwlock_unlock(&rwlock);
    }
}

int main(int argc, char **argv) {
    //freopen("in.txt", "r", stdin);
    //freopen("out.txt", "w", stdout);

    int i = 0, j, k;
    int test = 3, t = 0, kase = 0;
    int error, nt;

    if (argc > 2) {
        printf("illegal number of arguments: %d\n", argc);
        return 1;
    }

    nt = atoi(argv[1]);
//    if (nt != 2 && nt != 4 && nt != 8) {
//        printf("illegal number of threads to create: %d\n", nt);
//        return 1;
//    }

    double st = clock();
    pthread_rwlock_init(&rwlock, 0);
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

    for (i = 0; i < nt; i += 1) {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);

    pthread_rwlock_destroy(&rwlock);
    printf("%d threads took: %f\n", nt, (clock() - st) / CLOCKS_PER_SEC);

    return 0;
}
