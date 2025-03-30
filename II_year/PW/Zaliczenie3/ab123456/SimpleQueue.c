#include <malloc.h>
#include <pthread.h>
#include <stdatomic.h>

#include "SimpleQueue.h"

struct SimpleQueueNode;
typedef struct SimpleQueueNode SimpleQueueNode;

struct SimpleQueueNode {
    _Atomic(SimpleQueueNode*) next;
    Value item;
};

SimpleQueueNode* SimpleQueueNode_new(Value item)
{
    SimpleQueueNode* node = (SimpleQueueNode*)malloc(sizeof(SimpleQueueNode));
    atomic_init(&node->next, NULL);
    node->item = item;
    return node;
}

struct SimpleQueue {
    SimpleQueueNode* head;
    SimpleQueueNode* tail;
    pthread_mutex_t head_mtx;
    pthread_mutex_t tail_mtx;
};

SimpleQueue* SimpleQueue_new(void)
{
    SimpleQueue* queue = (SimpleQueue*)malloc(sizeof(SimpleQueue));
    SimpleQueueNode* node = SimpleQueueNode_new(TAKEN_VALUE);
    queue->head = node;
    queue->tail = node;
    pthread_mutex_init(&queue->head_mtx, NULL);
    pthread_mutex_init(&queue->tail_mtx, NULL);
    return queue;
}

void SimpleQueue_delete(SimpleQueue* queue)
{
    pthread_mutex_destroy(&queue->head_mtx);
    pthread_mutex_destroy(&queue->tail_mtx);
    SimpleQueueNode *node = queue->head;
    while(node != NULL) {
        SimpleQueueNode *temp = atomic_load(&node->next);
        free(node);
        node = temp;
    }
    free(queue);
}

void SimpleQueue_push(SimpleQueue* queue, Value item)
{
    pthread_mutex_lock(&queue->tail_mtx);   
    SimpleQueueNode *tail = queue->tail;
    SimpleQueueNode *newTail = SimpleQueueNode_new(item);
    atomic_store(&tail->next, newTail);
    queue->tail = newTail;
    pthread_mutex_unlock(&queue->tail_mtx); 
}

Value SimpleQueue_pop(SimpleQueue* queue)
{
    pthread_mutex_lock(&queue->head_mtx);  
    SimpleQueueNode *head = queue->head;
    if(head->next == NULL) {
        pthread_mutex_unlock(&queue->head_mtx); 
        return EMPTY_VALUE;
    }
    Value val = head->next->item;
    queue->head = head->next;
    queue->head->item = TAKEN_VALUE;
    pthread_mutex_unlock(&queue->head_mtx); 
    free(head);
    return val;
}

bool SimpleQueue_is_empty(SimpleQueue* queue)
{   
    pthread_mutex_lock(&queue->head_mtx);  
    bool val = queue->head->next == NULL;
    pthread_mutex_unlock(&queue->head_mtx); 
    
    return val;
}
