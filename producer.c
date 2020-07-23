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

void increaseProducersQuantity(int fileDescriptorProducers, char buf[]){
    struct stat shmobj_st;
    char *ptr;

    fileDescriptorProducers = shm_open (SM_PRODUCERS_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fileDescriptorProducers == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if(fstat(fileDescriptorProducers, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }

    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fileDescriptorProducers, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }

    //agarrar lo que hay en la memoria compartida
    int pq = atoi(ptr);
    pq = pq + 1;
    sprintf(buf, "%u", pq);

    ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fileDescriptorProducers, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }

    memcpy(ptr,buf, sizeof(buf));
    printf("%s \n", buf);
    close(fileDescriptorProducers);

}

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

    int randomKey = rand() % 5 + 0;

    sprintf(randomkeyC, "%u", randomKey);
    sprintf(message, "%u", pId);

    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char datetime[150];
    strftime(datetime,150,"%d/%m/%y %H:%M:%S",tlocal);
    printf("%s\n",datetime);

    strcat(message, "  ");
    strcat(message, randomkeyC);
    strcat(message, "  ");
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
    int fileDescriptorProducers;
    int pId;
    char buf[1];

    increaseProducersQuantity(fileDescriptorProducers, buf);

    mykey = atoi(argv[1]);
    inputTime = atoi(argv[2]);
    pId = atoi(argv[3]);

    shmid = shmget((key_t)mykey, sizeof(struct Queue), 0);

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

    srand((unsigned)time(NULL));
    //waitingTime = rand() % 10 + 2;

    printf("espera %d", waitingTime);

    while(1){
        if(queue->active == 1){
            printf("bloqueado \n");
            waitingTime = (rand() % inputTime + 2);
        }
        else{
            queue->active = 1;
            sem_wait(&queue->semaphore);
            printf("escribiendo \n");

            generateMessage(pId);

            node = (struct Node*)malloc(sizeof(struct Node));
            node->id = pId;
            strcpy(node->text, message);
            printf("%d", pId);
            enQueue(queue,*node);
            waitingTime = (rand() % inputTime + 2);
            queue->active = 0;
            sem_post(&queue->semaphore);
        }

        sleep(waitingTime);
        //delay(waitingTime);
        inputTime = inputTime+2;
    }

    /* Lastly, the shared memory is detached and then deleted. */
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

//    if (shmctl(shmid, IPC_RMID, 0) == -1) {
//        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
//        exit(EXIT_FAILURE);
//    }

    exit(EXIT_SUCCESS);
}
