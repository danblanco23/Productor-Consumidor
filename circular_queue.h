#define SIZE 100



typedef struct Node{
    int id;
    char text[100];
    
} Node;



typedef struct Queue{
    Node items[SIZE];
    int front;
    int back;
    int active;
} Queue;


int isFull(Queue* cola);
int isEmpty(Queue* cola);
void enQueue(Queue* cola,Node element);
Node *deQueue(Queue* cola);
void display(Queue* cola);
