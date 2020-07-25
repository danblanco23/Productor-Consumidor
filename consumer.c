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

#include <math.h>
#include <time.h>

#include "circular_queue.c"

#define SM_CONSUMERS_NAME  "/consumersQuantity"


int validateTermination(Node * consumed, int PID){
    int keyMessage = consumed->randomKey;
    int keyConsumer = PID % 5;

    if(keyMessage == keyConsumer){
        printf("\nMensaje de terminacion leido! PID modulo 5: %d , Llave del mensaje: %d\n", keyConsumer,keyMessage);
        return 0;
    } 
    else{
        return 1;
    }
}


int main(int argc, char* argv[])
{
    void *shared_memory = (void *)0;
    Queue *queue;
    int shmid;
    int key;
    int inputTime;
    int waitingTime;
    int consumerId;
    int totalWaitingTime = 0;
    int totalBlockedTime = 0;
    int totalMessagesConsumed = 0;
    int finishedKey = 1;


    key = atoi(argv[1]);
    inputTime = atoi(argv[2]);
    consumerId = atoi(argv[3]);

    srand((unsigned int)getpid());

    shmid = shmget((key_t)key, sizeof(struct Queue), 0);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    /* Hacemos memoria compartida accesible al programa */

    shared_memory = shmat(shmid, 0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    queue = (struct Queue *)shared_memory;
    queue->consumersQuantity = queue->consumersQuantity +1;

    while(queue->finish && finishedKey) {
        if(queue->active == 1){
            printf("         BLOQUEADO \n");
            waitingTime = (rand() % inputTime);
            totalBlockedTime += waitingTime;

        }
        else{
            queue->active = 1;
            sem_wait(&queue->semaphore);
            printf("\n........................Leyendo...................... \n");
            sleep(2);

            Node *readed = deQueue(queue);

            printf("\nIndice donde se leyo: %d\n", queue->first);
            printf("Productores Vivos: %d\n", queue->producersQuantity);
            printf("Consumidores Vivos: %d\n", queue->consumersQuantity);

            queue->totalMessagesConsumed = queue->totalMessagesConsumed +1;
            totalMessagesConsumed += 1;
            if (readed == NULL);
            {
                waitingTime = (rand() % inputTime);
                totalWaitingTime += waitingTime;
                queue->active = 0;
                sem_post(&queue->semaphore);
            }

            printf("Proceso: %d , Consumido: %s\n", consumerId, readed->text);
            finishedKey = validateTermination(readed, consumerId);
            waitingTime = (rand() % inputTime);
            queue->active = 0;
            sem_post(&queue->semaphore);
        }

        sleep(waitingTime);
        //delay(waitingTime);
        inputTime = inputTime+2;
    }

/* Lastly, the shared memory is detached and then deleted. */
    printf("Consumidor %d terminando ... bye!\n",consumerId);
    queue->consumersQuantity = queue->consumersQuantity -1;

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    //Imprimir estadisticas aca
    printf("\n*************************Estadisticas del consumidor %d*************************\n", consumerId);
    printf("Mensajes Consumidos: %d\n", totalMessagesConsumed);
    printf("Tiempo de espera total: %d\n", totalWaitingTime);
    printf("Tiempo bloqueado por semaforo total: %d\n", totalBlockedTime);

    exit(EXIT_SUCCESS);
}