#define SIZE 4



typedef struct Node{
    int id;
    char text[100];
    
} Node;



typedef struct Queue{
    Node items[SIZE];
} Queue;

int isFull(Queue* cola);
int isEmpty(Queue* cola);
void enQueue(Queue* cola,Node element);
int deQueue();
void display(Queue* cola);
