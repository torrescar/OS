#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"

struct element *buffer;
int maxSize;
int pos;

//To create a queue
int queue_init(int size){
    maxSize = size;
    pos = 0;
    
    buffer = (struct element *) malloc(size * sizeof(struct element));
    
    return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
    buffer[pos++] = *x;
    return 0;
}


// To Dequeue an element.
struct element* queue_get(void) {
    struct element *elem = malloc(sizeof(struct element));
    elem = &buffer[pos--];
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
    free(buffer);
	return 0;
}
