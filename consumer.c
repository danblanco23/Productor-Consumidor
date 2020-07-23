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
    int time;
    int processId;

    int fileDescriptorConsumers;
    char buf[1];

    increaseConsumersQuantity(fileDescriptorConsumers, buf);

    key = atoi(argv[1]);
    //time = atoi(argv[2]);
    //processId = atoi(argv[3]);

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
//    while(1) {
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

    int x = 1000;
    for(int i = 0; i < x; i++){

        sem_wait(&queue->semaphore);
        queue->aux = queue->aux - 1;
        sleep(1);
        printf(" XXXX %d", queue->aux);
        sem_post(&queue->semaphore);
        //sleep(2);
    }
    printf(" XXXX %d", queue->aux);


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