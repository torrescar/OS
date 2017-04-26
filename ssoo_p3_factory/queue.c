#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"

pthread_mutex_t mutex;
pthread_cond_t non_empty, non_full;
struct element *buffer;
int maxSize;
int pos;

//To create a queue
int queue_init(int size){
    maxSize = size;
    pos = 0;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&non_full, NULL);
    pthread_cond_init(&non_empty, NULL);
    
    buffer = (struct element *) malloc(size * sizeof(struct element));
    
    return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
    pthread_mutex_lock(&mutex);
    while(queue_full()) {
        pthread_cond_wait(&non_full, &mutex);
    }
    buffer[pos++] = *x;
    pthread_cond_signal(&non_empty);
    pthread_mutex_unlock(&mutex);
    return 0;
}


// To Dequeue an element.
struct element* queue_get(void) {
    pthread_mutex_lock(&mutex);
    while (queue_empty()) {
        pthread_cond_wait(&non_empty, &mutex);
    }
    struct element *elem = malloc(sizeof(struct element));
    elem = &buffer[pos--];
    pthread_cond_signal(&non_full);
    pthread_mutex_unlock(&mutex);
	return elem;
}


//To check queue state
int queue_empty(void){
	if (pos == 0)
        return 1;
    else
        return 0;
}

int queue_full(void){
	if (pos == maxSize)
        return 1;
    else
        return 0;
}

//To destroy the queue and free the resources
int queue_destroy(void){
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&non_full);
    pthread_cond_destroy(&non_empty);
    free(buffer);
	return 0;
}
