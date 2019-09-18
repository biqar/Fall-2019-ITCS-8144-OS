#pragma warning ( disable : 4786 )

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

const int inf = (1<<28);
const int MAX = 1005;
const int MAX_THREAD = 10;
const int THREAD_LOOP = 1000;
//const int THREAD_LOOP = 100000;
const int iterators[] = { 1, 10, 100 };
//const int iterators[] = { 1, 1000, 1000000 };

unsigned long int shared_resource;

thread threads[MAX_THREAD];
mutex mtx;

void thread_safe_increment(int tid, int itr) {
    printf("Job#%d has started\n", tid);

    for(int i=0; i<THREAD_LOOP; i+=1) {
        mtx.lock();
        printf("Job#%d acquired lock with shared_resource value: %lu\n", tid, shared_resource);
        for(int j=0; j<itr; j+=1) {
            shared_resource += 1;
        }
        printf("Job#%d releasing lock with shared_resource value: %lu\n", tid, shared_resource);
        mtx.unlock();
    }

    printf("Job#%d has finished\n", tid);

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
    if(nt != 2 && nt != 4 && nt != 8) {
        printf("illegal number of threads to create: %d\n", nt);
        return 1;
    }

    for(t=0; t<test; t+=1) {
        double st=clock();
        shared_resource = 0;
        for(i=0; i<nt; i+=1) {
            threads[i] = thread(thread_safe_increment, i, iterators[t]);
        }

        for(i=0; i<nt; i+=1) {
            threads[i].join();
        }
        printf("~~~~~~~~~~~~~done with %d iteration~~~~~~~~~~~~~\n", iterators[t]);
        cerr << nt << " threads with iteration " << iterators[t] << " took: " << (clock()-st)/CLOCKS_PER_SEC << endl;
    }

	return 0;
}
