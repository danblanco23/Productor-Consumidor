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


int main(int argc, char* argv[])
{
    void *shared_memory = (void *)0;
    Queue *queue;
    int shmid;
    int key;
    int inputTime;
    int waitingTime;
    int consumerId;
    int fileDescriptorConsumers;
    char buf[1];

    increaseConsumersQuantity(fileDescriptorConsumers, buf);

    key = atoi(argv[1]);
    inputTime = atoi(argv[2]);
    consumerId = atoi(argv[3]);

    srand((unsigned int)getpid());

    shmid = shmget((key_t)key, sizeof(struct Queue), 0);

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

    queue = (struct Queue *)shared_memory;

    while(queue->finish) {
        if(queue->active == 1){
            printf("bloqueado \n");
            waitingTime = (rand() % inputTime + 2);
        }
        else{
            queue->active = 1;
            sem_wait(&queue->semaphore);
            printf("leyendo \n");

            Node *readed = deQueue(queue);
            if (readed == NULL);
            {
                waitingTime = (rand() % inputTime + 2);
                queue->active = 0;
                sem_post(&queue->semaphore);
            }

            printf("Proceso: %d , Consumido: %s\n", consumerId, readed->text);

            waitingTime = (rand() % inputTime + 2);
            queue->active = 0;
            sem_post(&queue->semaphore);
        }

        sleep(waitingTime);
        //delay(waitingTime);
        inputTime = inputTime+2;
    }

//        sleep(2);
//        int rand_num = (rand() %  (time*2-1+1));
//        printf("Num rand: %d\n",rand_num);
//        if(time ==  rand_num && queue->active == 0){
//            printf("Num rand: %d\n",rand_num);
//            queue->active = 1;
//            Node *consumido = deQueue(queue);
//
//            printf("Proceso: %d , Consumido: %d\n", processId,consumido->id);
//            queue->active = 0;
//        }
//    }


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