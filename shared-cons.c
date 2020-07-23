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
#include <sys/sem.h>

#include "shared.h"
#include "circular_queue.c"

int semid;

struct sembuf sem_oper; 
union semun
 {
               int val;
               struct semid_ds *semstat;
               unsigned short *array;
  }
arg;

void wait(int id)
{
      sem_oper.sem_num=id;  //actuamos sobre el semáforo id
      sem_oper.sem_op=-1;   //decremento
      sem_oper.sem_flg=SEM_UNDO; // Para evitar interbloqueos
      semop(semid, &sem_oper,1);
}

void signal(int id)
{
      sem_oper.sem_num=id;  //actuamos sobre el semáforo id
      sem_oper.sem_op=1;    //incremento
      sem_oper.sem_flg=SEM_UNDO; // Para evitar interbloqueos
      semop(semid, &sem_oper,1);
}

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


    cola = (struct Queue *)shared_memory;


    semid=semget(rand(),1,0777|IPC_CREAT);

    // inicializar los semáforos
     
    arg.array=(unsigned short *) malloc(sizeof(short)*1);
        arg.array[0]=0;
        semctl (semid,1,SETALL,arg);

    
    while(cola->end) {

        int pid = fork();

        if(pid == 0){       //proceso hijo, liberador del semaforo
            sleep(2);
            int rand_num = (rand() %  (time*2-1+1));
            printf("Num rand: %d\n",rand_num);
            if(time ==  rand_num && cola->active == 0){
                printf("Puede consumir!\n");
                signal(0); //puedes consumir
            }
            if(time ==  rand_num && cola->active == 1){
                printf("La memoria compartida esta ocupada :(\n");
                
            }
        }
        else{   //proceso padre , en espera de semaforo para accesar a zona critica
            wait(0); // ver si se puede accesar
            printf("Accesando zona critica...\n");
            cola->active = 1;                                                    //Inicio critica
            Node *consumido = deQueue(cola);                                     //Zona critica
            printf("Proceso: %d , Consumido: %d\n", processID,consumido->id);    //Zona critica
            cola->active = 0;                                                    //Zona critica
            printf("Fin de zona critica...\n");
            wait(0);                                                             //Fin de zona critica 
                                                     
        }
    }
    printf("Consumidor %d terminando.. bye!\n", processID);



    // if (shmdt(shared_memory) == -1) {
    //     fprintf(stderr, "shmdt failed\n");
    //     exit(EXIT_FAILURE);
    // }

    // if (shmctl(shmid, IPC_RMID, 0) == -1) {
    //     fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    //     exit(EXIT_FAILURE);
    // }

    //exit(EXIT_SUCCESS);
}