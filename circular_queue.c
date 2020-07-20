
// Circular Queue implementation in C

#include <stdio.h>
#include "circular_queue.h"


// Check if the queue is full
int isFull(Queue* cola) {
  if ((cola->front == cola->back + 1) || (cola->front == 0 && cola->back == SIZE - 1)) return 1;
  return 0;
}

// Check if the queue is empty
int isEmpty(Queue* cola) {
  if (cola->front == -1) return 1;
  return 0;
}

// Adding an element
void enQueue(Queue* cola,Node element) {
  if (isFull(cola))
    printf("\n Queue is full!! \n");
  else {
    if (cola->front == -1) cola->front = 0;
    cola->back = (cola->back + 1) % SIZE;
    cola->items[cola->back] = element;
    //printf("\n Inserted -> %d", element);
  }
}

// Removing an element
Node *deQueue(Queue* cola) {
  Node *element;
  if (isEmpty(cola)) {
    printf("\n Queue is empty !! \n");
    return NULL;
  } else {
    element = &cola->items[cola->front];
    if (cola->front == cola->back) {
      cola->front = -1;
      cola->back = -1;
    } 
    // Q has only one element, so we reset the 
    // queue after dequeing it. ?
    else {
      cola->front = (cola->front + 1) % SIZE;
    }
    //printf("\n Deleted element -> %d \n", element);
    return (element);
  }
}

// Display the queue
void display(Queue* cola) {
  int i;
  if (isEmpty(cola))
    printf(" \n Empty Queue\n");
  else {
    printf("\n Front -> %d ", cola->front);
    printf("\n Items -> ");
    for (i = cola->front; i != cola->back; i = (i + 1) % SIZE) {
      printf("%d ", cola->items[i].id);
    }
    printf("%d ", cola->items[i].id);
    printf("\n Rear -> %d \n", cola->back);
  }
}