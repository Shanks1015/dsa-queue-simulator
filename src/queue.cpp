#include "queue.h"

void initQueue(Queue &q) {
    q.front = 0;
    q.rear = -1;
}

bool isEmpty(Queue &q) {
    return q.rear < q.front;
}

bool isFull(Queue &q) {
    return q.rear == MAX - 1;
}

void enqueue(Queue &q, Vehicle v) {
    if (!isFull(q)) {
        q.data[++q.rear] = v;
    }
}

Vehicle dequeue(Queue &q) {
    return q.data[q.front++];
}

int size(Queue &q) {
    return q.rear - q.front + 1;
}
