#include <stdio.h>
#include "circular_queue.h"


// Check if the queue is full
int isFull(Queue* queue) {
    if ((queue->first == queue->last + 1) || (queue->first == 0 && queue->last == SIZE - 1)) return 1;
    return 0;
}

// Check if the queue is empty
int isEmpty(Queue* queue) {
    if (queue->first == -1) return 1;
    return 0;
}

// Display the queue
void display(Queue* queue) {
    int i;
    if (isEmpty(queue))
        printf(" \n Empty Queue\n");
    else {
        printf("\n Front -> %d ", queue->first);
        printf("\n Items -> ");
        for (i = queue->first; i != queue->last; i = (i + 1) % SIZE) {
            printf("%d ", queue->items[i].id);
        }
        printf("%d ", queue->items[i].id);
        printf("\n Rear -> %d \n", queue->last);
    }
}

// Adding an element
void enQueue(Queue* queue,Node node) {
    if (isFull(queue)){
        printf("\n Queue is full!! \n");
        display(queue);
    }

    else {
        if (queue->first == -1) queue->first = 0;
        queue->last = (queue->last + 1) % SIZE;
        queue->items[queue->last] = node;
    }
}

// Removing an element
Node *deQueue(Queue* queue) {
    Node *element;
    if (isEmpty(queue)) {
        printf("\n Queue is empty !! \n");
        return NULL;
    } else {
        element = &queue->items[queue->first];
        if (queue->first == queue->last) {
            queue->first = -1;
            queue->last = -1;
        }
            // Q has only one element, so we reset the 
            // queue after dequeing it. ?
        else {
            queue->first = (queue->first + 1) % SIZE;
        }
        return (element);
    }
}

