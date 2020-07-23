#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "shared.h"
#include "circular_queue.c"
int main()
{
    int key;
    int time;
    //Pide comandos
    printf("Shared memory id: ");
    scanf("%d",&key);
   
    Queue *cola;
    int shmid;
    void *shared_memory = (void *)0;

    shmid = shmget(key, sizeof(struct Queue), 0);
    shared_memory = shmat(shmid, 0, 0);
    cola = (struct Queue *)shared_memory;
    cola->end = 0;

    //Liberamos memoria compartida
    shmctl(shmid, IPC_RMID, NULL); 

    //De-attach
    shmdt(shared_memory);
    printf("Deleted shared memory!\n");
    
    return 0;
}