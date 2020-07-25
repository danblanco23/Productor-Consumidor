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


void increaseConsumersQuantity(int fileDescriptorConsumers, char buf[]){

    struct stat shmobj_st;
    char *ptr;

    fileDescriptorConsumers = shm_open (SM_CONSUMERS_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fileDescriptorConsumers == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if(fstat(fileDescriptorConsumers, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }

    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fileDescriptorConsumers, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }

    int pq = atoi(ptr);
    pq = pq + 1;
    sprintf(buf, "%u", pq);

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
int validateTermination(Node * consumed, int PID){
    int keyMessage = consumed->randomKey;
    int keyConsumer = PID % 5;

    if(keyMessage == keyConsumer){
        printf("Mensaje de terminacion leido! PID modulo 5: %d , Llave del mensaje: %d\n", keyConsumer,keyMessage);
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
    char buf[1];
    int tiempoEspera = 0;
    int totalConsumido = 0;


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
    queue->consumidores = queue->consumidores +1;

    while(queue->finish) {
        if(queue->active == 1){
            printf("bloqueado \n");
            waitingTime = (rand() % inputTime + 2);
            tiempoEspera += waitingTime;

        }
        else{
            queue->active = 1;
            sem_wait(&queue->semaphore);
            printf("leyendo \n");

            Node *readed = deQueue(queue);
            queue->mensajesConsumidos = queue->mensajesConsumidos +1;
            if (readed == NULL);
            {
                waitingTime = (rand() % inputTime + 2);
                tiempoEspera += waitingTime;
                queue->active = 0;
                sem_post(&queue->semaphore);
            }

            printf("Proceso: %d , Consumido: %s\n", consumerId, readed->text);
            queue->finish = validateTermination(readed, consumerId);
            waitingTime = (rand() % inputTime + 2);
            queue->active = 0;
            sem_post(&queue->semaphore);
        }

        sleep(waitingTime);
        //delay(waitingTime);
        inputTime = inputTime+2;
    }

/* Lastly, the shared memory is detached and then deleted. */
    printf("Consumidor %d terminando ... bye!\n",consumerId);
    queue->consumidores = queue->consumidores -1;

    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    //Imprimir estadisticas aca
    printf("*************************Estadisticas del consumidor %d*************************\n", consumerId);
    printf("Mensajes producidos: %d\n", totalConsumido);
    printf("Tiempo bloqueado por semaforo: %d\n", tiempoEspera);
    exit(EXIT_SUCCESS);
}