#include "stdlib.h"
#include "circular_queue.h"
void createQueue(int length){
    int i = 0;

    while(length > 0){
        Node* newNode = (Node*) malloc(sizeof(Node));

        if(first == NULL){
            newNode->id = i;
            first = newNode;
            first->next = first;
            last = first;
        } else{
            newNode->id = i;
            last->next = newNode;
            newNode->next = first;
            last = newNode;
        }
        length--;
        i++;
    }
}