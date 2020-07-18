typedef struct Node{
    int id;
    struct Node *next;
} Node;

Node* first = NULL;
Node* last = NULL;

void createQueue(Process process);
struct Process dequeue();
