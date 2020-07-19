
// Circular Queue implementation in C

#include <stdio.h>
#include "circular_queue.h"

int front = -1, rear = -1;

// Check if the queue is full
int isFull(Queue* cola) {
  if ((front == rear + 1) || (front == 0 && rear == SIZE - 1)) return 1;
  return 0;
}

// Check if the queue is empty
int isEmpty(Queue* cola) {
  if (front == -1) return 1;
  return 0;
}

// Adding an element
void enQueue(Queue* cola,Node element) {
  if (isFull(cola))
    printf("\n Queue is full!! \n");
  else {
    if (front == -1) front = 0;
    rear = (rear + 1) % SIZE;
    cola->items[rear] = element;
    //printf("\n Inserted -> %d", element);
  }
}

// // Removing an element
// Node *deQueue(Queue* cola) {
//   Node element;
//   if (isEmpty(cola)) {
//     printf("\n Queue is empty !! \n");
//     return NULL;
//   } else {
//     element = cola->items[front];
//     if (front == rear) {
//       front = -1;
//       rear = -1;
//     } 
//     // Q has only one element, so we reset the 
//     // queue after dequeing it. ?
//     else {
//       front = (front + 1) % SIZE;
//     }
//     printf("\n Deleted element -> %d \n", element);
//     return (element);
//   }
// }

// Display the queue
void display(Queue* cola) {
  int i;
  if (isEmpty(cola))
    printf(" \n Empty Queue\n");
  else {
    printf("\n Front -> %d ", front);
    printf("\n Items -> ");
    for (i = front; i != rear; i = (i + 1) % SIZE) {
      //printf("%d ", cola->items[i]);
    }
    //printf("%d ", cola->items[i]);
    printf("\n Rear -> %d \n", rear);
  }
}