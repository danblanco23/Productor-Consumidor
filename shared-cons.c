/* Our first program is a consumer. After the headers the shared memory segment
 (the size of our shared memory structure) is created with a call to shmget,
 with the IPC_CREAT bit specified. */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"
#include "circular_queue.c"

int main()
{
    int key;
    int time;
    int processID;
    //Pide comandos
    printf("Shared memory id: ");
    scanf("%d",&key);

    printf("Tiempo de espera: ");
    scanf("%d",&time);

    printf("Process number: ");
    scanf("%d",&processID);

    int running = 1;
    void *shared_memory = (void *)0;
    struct Node *object;
    Queue *cola;
    
    int shmid;
    
    shmid = shmget(key, sizeof(struct Queue), 0);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

/* We now make the shared memory accessible to the program. */

    shared_memory = shmat(shmid, 0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    //printf("Memory attached at %X\n", (int)shared_memory);

/* The next portion of the program assigns the shared_memory segment to shared_stuff,
 which then prints out any text in written_by_you. The loop continues until end is found
 in written_by_you. The call to sleep forces the consumer to sit in its critical section,
 which makes the producer wait. */

    cola = (struct Queue *)shared_memory;
    while(running) {
        sleep(2);
        int rand_num = (rand() %  (time*2-1+1));
        printf("Num rand: %d\n",rand_num);
        if(time ==  rand_num && cola->active == 0){
            printf("Num rand: %d\n",rand_num);
            cola->active = 1;
            Node *consumido = deQueue(cola);
            
            printf("Proceso: %d , Consumido: %d\n", processID,consumido->id);
            cola->active = 0;

        }
        
    }
    
    // while(running) {
    //     if (shared_stuff->written_by_you) {
    //         printf("You wrote: %s", shared_stuff->some_text);
    //         sleep( rand() % 4 ); /* make the other process wait for us ! */
    //         shared_stuff->written_by_you = 0;
    //         if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
    //             running = 0;
    //         }
    //     }
    // }

/* Lastly, the shared memory is detached and then deleted. */

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}