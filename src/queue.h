#ifndef QUEUE_H
#define QUEUE_H

#define MAX 100

struct Vehicle {
    int id;
    char road;
    int lane;
};

struct Queue {
    Vehicle data[MAX];
    int front;
    int rear;
};

void initQueue(Queue &q);
int isEmpty(Queue &q);
int isFull(Queue &q);
void enqueue(Queue &q, Vehicle v);
Vehicle dequeue(Queue &q);
int size(Queue &q);

#endif
