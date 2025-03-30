#include <malloc.h>
#include <pthread.h>
#include <stdatomic.h>

#include <stdio.h>
#include <stdlib.h>

#include "HazardPointer.h"
#include "RingsQueue.h"

struct RingsQueueNode;
typedef struct RingsQueueNode RingsQueueNode;

struct RingsQueueNode {
    _Atomic(RingsQueueNode*) next;
    Value buf[RING_SIZE];
    _Atomic(long long) push_idx;
    _Atomic(long long) pop_idx;    
};

RingsQueueNode* RingsQueueNode_new() {
    RingsQueueNode *newNode = (RingsQueueNode*)malloc(sizeof(RingsQueueNode));
    // newNode->buf = (Value*)malloc(sizeof(Value)*RING_SIZE);
    atomic_init(&newNode->next, NULL);
    atomic_init(&newNode->push_idx, 0);
    atomic_init(&newNode->pop_idx, 0);
    return newNode;
}

struct RingsQueue {
    RingsQueueNode* head;
    RingsQueueNode* tail;
    pthread_mutex_t pop_mtx;
    pthread_mutex_t push_mtx;
};

RingsQueue* RingsQueue_new(void)
{
    RingsQueue* queue = (RingsQueue*)malloc(sizeof(RingsQueue));
    RingsQueueNode *node = RingsQueueNode_new();
    queue->head = node;
    queue->tail = node;
    pthread_mutex_init(&queue->pop_mtx, NULL);
    pthread_mutex_init(&queue->push_mtx, NULL);
    return queue;
}

void RingsQueue_delete(RingsQueue* queue)
{
    pthread_mutex_destroy(&queue->pop_mtx);
    pthread_mutex_destroy(&queue->push_mtx);
    RingsQueueNode *node = queue->head;
    while(node != NULL) {
        RingsQueueNode *temp = atomic_load(&node->next);
        free(node);
        node = temp;
    }
    free(queue);
}

void RingsQueue_push(RingsQueue* queue, Value item)
{
    pthread_mutex_lock(&queue->pop_mtx);
    RingsQueueNode *tail = queue->tail;
    
    if(atomic_load(&tail->push_idx) - atomic_load(&tail->pop_idx) == RING_SIZE) {
        RingsQueueNode *newTail = RingsQueueNode_new();
        atomic_store(&tail->next, newTail);
        tail = newTail;
        queue->tail = newTail;
    }
    tail->buf[atomic_fetch_add(&tail->push_idx, 1)%RING_SIZE] = item;
    pthread_mutex_unlock(&queue->pop_mtx);
}

Value RingsQueue_pop(RingsQueue* queue)
{
    pthread_mutex_lock(&queue->pop_mtx);
    RingsQueueNode *head = queue->head;
    RingsQueueNode *next = atomic_load(&head->next);
    if(atomic_load(&head->pop_idx) == atomic_load(&head->push_idx)) {
        if(next == NULL) {
            pthread_mutex_unlock(&queue->pop_mtx);
            return EMPTY_VALUE;
        }
        RingsQueueNode *newHead = next;
        free(head);
        head = newHead;
        queue->head = newHead;
    }
    Value val = head->buf[atomic_fetch_add(&head->pop_idx, 1)%RING_SIZE];
    pthread_mutex_unlock(&queue->pop_mtx);
    return val;
}

bool RingsQueue_is_empty(RingsQueue* queue)
{   pthread_mutex_lock(&queue->pop_mtx);
    RingsQueueNode *head = queue->head;
    bool val = atomic_load(&head->push_idx) == atomic_load(&head->pop_idx);
    pthread_mutex_unlock(&queue->pop_mtx);
    return val;
}


