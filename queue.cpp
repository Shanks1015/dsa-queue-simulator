#include "queue.h"

void initQueue(Queue &q) {
    q.front = 0;
    q.rear = -1;
}

int isEmpty(Queue &q) {
    return q.front > q.rear;
}

int isFull(Queue &q) {
    return q.rear == MAX - 1;
}

void enqueue(Queue &q, Vehicle v) {
    if (!isFull(q)) {
        q.data[++q.rear] = v;
    }
}

Vehicle dequeue(Queue &q) {
    Vehicle v = {-1, 'X', -1};
    if (!isEmpty(q)) {
        v = q.data[q.front++];
    }
    return v;
}

int size(Queue &q) {
    return q.rear - q.front + 1;
}
