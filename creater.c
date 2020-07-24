#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#include "circular_queue.c"

#define SIZEOF_SM_PRODUCERS 100
#define SM_PRODUCERS_NAME  "/producersQuantity"

#define SIZEOF_SM_CONSUMERS 100
#define SM_CONSUMERS_NAME  "/consumersQuantity"


void sharedMemoryProducersQuantity(int fileDescriptorProducers, char buf[]){
    char *ptr;

    fileDescriptorProducers = shm_open(SM_PRODUCERS_NAME, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
    if(fileDescriptorProducers == -1)
    {
        printf("Error file descriptor \n");
        exit(1);
    }
    if(-1 == ftruncate(fileDescriptorProducers, SIZEOF_SM_PRODUCERS))
    {
        printf("Error shared memory cannot be resized \n");
        exit(1);
    }

    ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fileDescriptorProducers, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }

    memcpy(ptr,buf, sizeof(buf));
    printf("%s \n", buf);
    close(fileDescriptorProducers);
}

void sharedMemoryConsumersQuantity(int fileDescriptorConsumers, char buf[]){
    char *ptr;

    fileDescriptorConsumers = shm_open(SM_CONSUMERS_NAME, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
    if(fileDescriptorConsumers == -1)
    {
        printf("Error file descriptor \n");
        exit(1);
    }
    if(-1 == ftruncate(fileDescriptorConsumers, SIZEOF_SM_CONSUMERS))
    {
        printf("Error shared memory cannot be resized \n");
        exit(1);
    }

    ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fileDescriptorConsumers, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }

    memcpy(ptr,buf, sizeof(buf));
    printf("%s \n", buf);
    close(fileDescriptorConsumers);
}

int main(int argc, char* argv[]) {

    void *shared_memory = (void *)0;
    Queue *queue;
    int shmid;
    int mykey;
    int fileDescriptorProducers;
    int fileDescriptorConsumers;
    char bufferProducers[] = "0";
    char bufferConsumers[] = "0";

    mykey = atoi(argv[1]);

    shmid = shmget((key_t)mykey, sizeof(struct Queue), 0666 | IPC_CREAT);
    printf("%d \n", shmid);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, 0, 0);  //id, direccion de inicio, bandera read,write,exec
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    queue = (struct Queue *)shared_memory;
    queue->last = -1;
    queue->first = -1;
    queue->active = 0;
    queue->finish = 1;

    if (sem_init(&queue->semaphore, 1, 1) == 0)
        fprintf(stderr, "sem failed\n");

    sem_wait(&queue->semaphore);
    sem_post(&queue->semaphore);

    sharedMemoryProducersQuantity(fileDescriptorProducers, bufferProducers);
    sharedMemoryConsumersQuantity(fileDescriptorConsumers, bufferConsumers);

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

//    if (shmctl(shmid, IPC_RMID, 0) == -1) {
//        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
//        exit(EXIT_FAILURE);
//    }
    return 0;
}
