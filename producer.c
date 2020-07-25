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

#define SM_PRODUCERS_NAME  "/producersQuantity"

#define TEXT_SZ 2048

char message[200];
int randomKey;


double ran_expo(int time){
    double u;
    u = rand() / (time + 2.0);
    return -log(1- u) / 0.05;
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}

void generateMessage(int pId)
{
    char randomkeyC[2];
    char idC[2];

    randomKey = rand() % 5 + 0;

    sprintf(randomkeyC, "%u", randomKey);
    sprintf(idC, "%u", pId);
    sprintf(message, "%u", pId);

    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char datetime[150];
    strftime(datetime,150,"%d/%m/%y %H:%M:%S",tlocal);
    //printf("%s\n",datetime);


    strcat(message, "Id: ");
    strcat(message, idC);
    strcat(message, ", Llave generada:");
    strcat(message, randomkeyC);
    strcat(message, ", Fecha y hora: ");
    strcat(message, datetime);

    printf("%s", message);
}


int main(int argc, char* argv[])
{
    void *shared_memory = (void *)0;
    struct Node *object;
    Queue *queue;
    struct Node *node;
    int shmid;
    int mykey;
    int inputTime;
    int waitingTime;
    int producerId;
    int totalWaitingTime = 0;
    int totalBlockedTime = 0;
    int totalMessagesProduced = 0;


    mykey = atoi(argv[1]);
    inputTime = atoi(argv[2]);
    producerId = atoi(argv[3]);

    shmid = shmget((key_t)mykey, sizeof(struct Queue), 0);

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
    queue->producersQuantity = queue->producersQuantity +1;
    srand((unsigned)time(NULL));
    //waitingTime = rand() % 10 + 2;

    printf("espera %d", waitingTime);

    while(queue->finish){
        if(queue->active == 1){
            printf("bloqueado \n");
            waitingTime = (rand() % inputTime);
            totalBlockedTime += waitingTime;
        }
        else{
            queue->active = 1;
            sem_wait(&queue->semaphore);
            printf("\n....................Escribiendo................... \n");

            sleep(2);

            generateMessage(producerId);

            node = (struct Node*)malloc(sizeof(struct Node));
            node->id = producerId;
            node->randomKey = randomKey;
            strcpy(node->text, message);
            //printf("%d", producerId);
            enQueue(queue,*node);

            printf("\nIndice donde se escribio: %d\n", queue->last);
            printf("Productores Vivos: %d\n", queue->producersQuantity);
            printf("Consumidores Vivos: %d\n", queue->consumersQuantity);

            queue->totalMessagesProduced = queue->totalMessagesProduced +1;
            totalMessagesProduced += 1;

            display(queue);
            waitingTime = (rand() % inputTime);
            totalWaitingTime += waitingTime;
            queue->active = 0;
            sem_post(&queue->semaphore);
        }

        sleep(waitingTime);
        printf("%d", waitingTime);
        //delay(waitingTime);
        inputTime = inputTime+2;
    }
    printf("Productor %d terminando ... bye!\n\n",producerId);
    queue->producersQuantity = queue->producersQuantity -1;
    /* Lastly, the shared memory is detached and then deleted. */
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    //Imprimir estadisticas aca
    printf("*************************Estadisticas del productor %d*************************\n", producerId);
    printf("Mensajes producidos: %d\n", totalMessagesProduced);
    printf("Tiempo de espera total: %d\n", totalWaitingTime);
    printf("Tiempo bloqueado por semaforo total: %d\n", totalBlockedTime);

    exit(EXIT_SUCCESS);
}
