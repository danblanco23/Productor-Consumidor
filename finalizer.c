#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "circular_queue.c"
int main(int argc, char* argv[])
{
    int shmid;
    void *shared_memory = (void *)0;
    int key;
    int time;

    key = atoi(argv[1]);

    Queue *queue;

    shmid = shmget((key_t)key, sizeof(struct Queue), 0);
    shared_memory = shmat(shmid, 0, 0);

    queue = (struct Queue *)shared_memory;
    queue->finish = 0;
    printf("*************************Estadisticas Globales*************************\n");
    printf("Total productores creados: %d\n", queue->producersQuantity);
    printf("Total consumidores creados: %d\n", queue->consumersQuantity);
    printf("Total mensajes producidos: %d\n", queue->totalMessagesProduced);
    printf("Total mensajes consumidos: %d\n", queue->totalMessagesConsumed);

    //Liberamos memoria compartida
    shmctl(shmid, IPC_RMID, NULL);

    //De-attach
    shmdt(shared_memory);
    printf("Deleted shared memory!\n\n");

  


    return 0;
}