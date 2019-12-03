//
// Created by aislam6 on 12/2/19.
//

/**
 * https://stackoverflow.com/questions/22091555/what-is-an-easy-way-to-trigger-a-page-fault
 * */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    long pagesize = sysconf(_SC_PAGESIZE);
    printf("program process-id: %d\n", getpid());

    /* Cross page boundaries. Page fault may occur depending on your allocator / libc implementation. */
    unsigned char *p = (unsigned char *) malloc(pagesize + 1);

    p[0] = 0;        /* Page fault. */
    p[pagesize] = 1; /* Page fault. */

    return 0;
}