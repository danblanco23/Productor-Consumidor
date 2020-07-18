#include "circular_queue.h"
void createQueue(int length){
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->process = process;
    int i = 0;

    while(length > 0){
        if(length == 1){
            last->next = newNode;
            newNode->next = first;
            last = newNode;
            newNode->id = i;
        }
        if(first == NULL){
            first = newNode;
            first->next = NULL;
            last = newNode;
            newNode->id = i;
        } else{
            last->next = newNode;
            newNode->next = NULL;
            last = newNode;
            newNode->id = i;
        }
        length--;
        i++;
    }


}

struct Process dequeue(){
    Node* aux = (Node*) malloc(sizeof(Node));
    aux = first;
    struct Process process;

    if(aux != NULL){                    //Si solo tiene un elemento
        if(aux->next == NULL){
            first = NULL;
            last = NULL;
            process = aux->process;
            return process;
        }
        else{                          //Si hay mas de un elemento
            first = aux->next;
            process = aux->process;
            //free(aux);
            return process;
        }

    } else{
        printf("no hay colaaaa");
    }
}