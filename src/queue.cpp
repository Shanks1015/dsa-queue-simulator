#include "queue.h"
#include <iostream>
using namespace std;

void initQueue(Queue &q) {
    q.front = -1;
    q.rear = -1;
}

bool isEmpty(Queue &q) {
    return q.front == -1;
}

bool isFull(Queue &q) {
    return (q.rear + 1) % MAX == q.front;
}

void enqueue(Queue &q, Vehicle v) {
    if (isFull(q)) {
        cout << "Queue is full! Cannot enqueue vehicle " << v.id << endl;
        return;
    }
    
    if (isEmpty(q)) {
        q.front = 0;
        q.rear = 0;
    } else {
        q.rear = (q.rear + 1) % MAX;
    }
    
    q.data[q.rear] = v;
}

Vehicle dequeue(Queue &q) {
    Vehicle v = {-1, 'X', -1};
    
    if (isEmpty(q)) {
        return v;
    }
    
    v = q.data[q.front];
    
    if (q.front == q.rear) {
        q.front = -1;
        q.rear = -1;
    } else {
        q.front = (q.front + 1) % MAX;
    }
    
    return v;
}

int size(Queue &q) {
    if (isEmpty(q)) {
        return 0;
    }
    
    if (q.rear >= q.front) {
        return q.rear - q.front + 1;
    } else {
        return MAX - q.front + q.rear + 1;
    }
}
