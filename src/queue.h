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
bool isEmpty(Queue &q);
bool isFull(Queue &q);
void enqueue(Queue &q, Vehicle v);
Vehicle dequeue(Queue &q);
int size(Queue &q);

#endif
