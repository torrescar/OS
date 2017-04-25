/*
 *
 * process_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>
#include <ctype.h>

#define NUM_THREADS 2

/*//Thread function
void *PrintHello(void *threadid)
{
   	long tid;
   	tid = (long)threadid;
   	printf("Hello World! It's me, thread #%ld!\n", tid);
	printf("Thread #%ld ends\n", tid);
   	pthread_exit(0);
}*/

int toMove, beltID;

/*The helper method returns 1 if parameter is number. Otherwise, it returns 0.*/
int isInt(const char param[]){
    if (param == NULL) {
        return 0;
    }
    int index;
    for(index=0; param[index]!= '\0'; index++){
        if(!isdigit(param[index])){
            if(param[index]!='-'){
                return 0;
            }
        }
        
    }
    return 1;
}

void * producer(void *arg){
    // producer produces all elements its responsible for (argv[4] num of elements) and insert in belt
    int i;
    for (i = 0; i < toMove; i++) {
        struct element x;
        x.num_edition = i;
        x.id_belt = beltID;
        if (i == toMove -1) {
            x.last = 1;
        }
        else {
            x.last = 0;
        }
        queue_put(&x);
        printf("[OK] [queue] Introduced element with id: %d in belt %d.\n", i, beltID);
    }
    printf("[OK] [process_manager] Process_manager with id: %d has produced %d elements.\n", beltID, toMove);
    pthread_exit(0);
}

void * consumer(void *arg){
    // consumer consumes all elements its responsible for and remove from belt
    int j;
    for (j = 0; j < toMove; j++) {
        queue_get();
        printf("[OK] [queue] Obtained element with id: %d in belt %d.\n", j, beltID);
    }
    pthread_exit(0);
}


// argv[1] = id
// argv[2] = char pointer representing semaphore name
// argv[3] = max size of belt
// argv[4] = no. products to move

int main (int argc, const char * argv[] ){
    // Check arguments validity
    if (!isInt(argv[1]) || !isInt(argv[3]) || !isInt(argv[4])) {
        printf("[ERROR][process_manager] Arguments not valid\n");
    }
    
	// Wait until semaphore (argv[2]) is unlocked
    /*if ((sem_t s = sem_open(argv[2], 0)) == -1) {
        printf("[ERROR][process_manager] Arguments not valid");
    }t
    sem_t s;
    sem_init(&s, 0, 1);*/
    
    toMove = atoi(argv[4]);
    beltID = atoi(argv[1]);
    
    printf("[OK] [process_manager] Process_manager with id: %s waiting to produce %s elements.\n", argv[1], argv[4]);
    //sem_wait(&s);
    
    // create belt of max size argv[3]
    queue_init(atoi(argv[3]));
    printf("[OK] [process_manager] Belt with id: %s has been created with a maximum of %s elements.\n", argv[1], argv[3]);
    
    // create two threads:
    pthread_t prod, cons;
    
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    
    // Destroy all associated resources
    queue_destroy();
    //sem_post(&s);
    return 0;
	
}
