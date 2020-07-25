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


int main(int argc, char* argv[]) {

    void *shared_memory = (void *)0;
    Queue *queue;
    int shmid;
    int mykey;

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
    queue = (Queue *) malloc(sizeof(struct Queue *));
    queue = (struct Queue *)shared_memory;
    queue->last = -1;
    queue->first = -1;
    queue->active = 0;
    queue->finish = 1;
    queue->consumidores = 0;
    queue->productores = 0;
    queue->mensajesConsumidos = 0;
    queue->mensajesProducidos = 0;

    if (sem_init(&queue->semaphore, 1, 1) == 0)
        fprintf(stderr, "sem failed\n");

    sem_wait(&queue->semaphore);
    sem_post(&queue->semaphore);


    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }


    return 0;
}
