#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O* constants */
#include <unistd.h>
#include <sys/types.h>
#define SIZEOF_SMOBJ 200
//#define SMOBJ_NAME  "/myMemoryObj"

int main(int argc, char* argv[]) {
    int fd;
    char* smobj_name = argv[1];

    fd = shm_open(smobj_name, O_CREAT | O_RDWR  , 00600); /* create s.m object*/
    if(fd == -1)
    {
        printf("Error file descriptor \n");
        exit(1);
    }
    if(-1 == ftruncate(fd, SIZEOF_SMOBJ))
    {
        printf("Error shared memory cannot be resized \n");
        exit(1);
    }

    close(fd);

    return 0;
}
