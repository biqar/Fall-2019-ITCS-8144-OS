//
// Created by aislam6 on 12/2/19.
//

/**
 * Helper program to generate major page faults.
 *
 * Procedure: https://unix.stackexchange.com/questions/188170/generate-major-page-faults
 * To produce major page faults you need to force reads from disk.
 * The following code maps the file given on the command-line, telling the kernel it doesn't need it;
 * if the file is large enough (pick a large file in /usr/bin), you'll always get major page faults:
 * */

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FILE_NAME "/usr/bin/time"

int main(int argc, char **argv) {
    int fd = open(FILE_NAME, O_RDONLY);
    struct stat stats;
    fstat(fd, &stats);
    posix_fadvise(fd, 0, stats.st_size, POSIX_FADV_DONTNEED);
    char *map = (char *) mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Failed to mmap");
        return 1;
    }
    int result = 0;
    int i;
    for (i = 0; i < stats.st_size; i++) {
        result += map[i];
    }
    munmap(map, stats.st_size);
    return result;
}
