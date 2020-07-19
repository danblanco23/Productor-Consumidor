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
    int running = 1;
    void *shared_memory = (void *)0;
    struct Node *object;
    Queue *cola;
    char buffer[TEXT_SZ ];
    int shmid;
    int mykey = getuid();

    //createQueue(3);

    //printf("Nodo %d",first->next->id);
    shmid = shmget((key_t)mykey, sizeof(struct Queue), 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, 0, 0);  //id, direccion de inicio, bandera read,write,exec
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    //printf("Memory attached at %X\n", (int)shared_memory);

    cola = (struct Queue *)shared_memory;
    cola->items[0].id =3;
    cola->items[1].id =10;
    
    
    while(running){
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
    }
    // while(running) {
    //     // while(shared_stuff->written_by_you == 1) {
    //     //     sleep(1);            
    //     //     printf("waiting for client...\n");
    //     // }
    //     printf("Enter some text: ");
    //     fgets(buffer, BUFSIZ, stdin);
        
    //     //strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
    //     shared_stuff->written_by_you = 1;

    //     if (strncmp(buffer, "end", 3) == 0) {
    //             running = 0;
    //     }
    // }

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
