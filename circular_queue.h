#define SIZE 20
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node{
    int id;
    int randomKey;
    char text[300];

} Node;

typedef struct Queue{
    Node items[SIZE];
    int first;
    int last;
    int active;
    int finish;
    sem_t semaphore;
    int producersQuantity;
    int consumersQuantity;
    int totalMessagesProduced;
    int totalMessagesConsumed;
} Queue;

int isFull(Queue* queue);
int isEmpty(Queue* queue);
void enQueue(Queue* queue,Node node);
Node *deQueue(Queue* queue);
void display(Queue* queue);