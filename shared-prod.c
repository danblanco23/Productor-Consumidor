/* The second program is the producer and allows us to enter data for consumers.
 It's very similar to shm1.c and looks like this. */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"
#include "circular_queue.c"
int main()
{
    int key;
    int time;
    //Pide comandos
    printf("Shared memory id: ");
    scanf("%d",&key);

    printf("Tiempo de espera: ");
    scanf("%d",&time);





    int running = 1;
    void *shared_memory = (void *)0;
    struct Node *object;
    Queue *cola;
    int shmid;

    shmid = shmget(key, sizeof(struct Queue), 0666 | IPC_CREAT); //setea memoria compartida

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, 0, 0);  //id, direccion de inicio, bandera read,write,exec , la vuelve usable
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    int cont = 0;
    cola = (struct Queue *)shared_memory;
    cola->back = -1;
    cola->front = -1;
    cola->end=1;
    
    while(cont<=50){
        
        object = (struct Node*)malloc(sizeof(struct Node));
        object->id=cont;
        strcpy(object->text,"Texto");
        enQueue(cola,*object);
        cont+=1;
    }
    cola->active =0;
    while(running){


    }
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
