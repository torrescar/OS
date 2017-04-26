//How to initialize a process manager instance
/*Create semaphore for each process manager, create process manager instance, use signal/wait to have the process managers execute in order*/
//create child depending on number of process managers, and each child executes program pm,
//create a semaphore for each process

/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>


#define BUFFER_SIZE 1024



int main (int argc, const char * argv[]){
    char buf[BUFFER_SIZE];
    int maxNumPM;
    int fd,n;
    
    /*If the number of arguments is less than two, we print an error message and return -1.*/
    if(argc<2){
        perror("[ERROR][factory_manager] Invalid file1\n");
        return -1;
    }
    
    /*If the value of the file descriptor returned from opening the given file is less than 0, we print an error message and return -1.*/
    if ((fd=open(argv[1],O_RDONLY,0777))<0) {
        perror("[ERROR][factory_manager] Invalid file2\n");
        return -1;
    }
    
    /*The contents of the file are read into our immediate buffer, buf, of 1024 bytes. If an error occurs, an error message is printed and return -1.*/
    n=read(fd,buf,BUFFER_SIZE);
    
    printf("%s",buf);
    printf("\n");

    if(n<0){
        perror("[ERROR][factory_manager] Invalid file3\n");
        return -1;
    }
    
    close(fd);
    maxNumPM= atoi(&buf[0]);
    
    
    /*The maximum number of belts is checked for each process manager to make sure it is not equal or less than 0. If it is, an error message is printed and return -1.*/
    int index=2;
    int actualNumPM=0;
    while(strcmp(&buf[index],"\0")!=0){
        actualNumPM++;
        if(atoi(&buf[index+2])<=0){
            perror("[ERROR][factory_manager] Invalid file4\n");
            return -1;
        }
        index=index+6;
    }
    
    /*If there are more process managers than the maximum number allowed, an error message is printed and return -1.*/
    if(actualNumPM>maxNumPM){
        perror("[ERROR][factory_manager] Invalid file6\n");
        return -1;
    }
    
    /*For each process manager, a semaphore is created and the helper method createProcessManager is called.*/
    int index2=2;
    while(strcmp(&buf[index2],"\0")!=0){

        int idN= atoi(&buf[index2]);
        int sizeOfBeltN= atoi(&buf[index2+2]);
        int numElemN= atoi(&buf[index2+4]);
        
        char id[100], sizeOfBelt[100], numElem[100], name[100];

        sprintf(id, "%d", idN);
        sprintf(sizeOfBelt, "%d", sizeOfBeltN);
        sprintf(numElem, "%d", numElemN);
        sprintf(name, "/%d", index2);
        
        sem_t *s = sem_open(name, O_CREAT, 0777, 1);
        if(index2==2){
            s = sem_open(name, O_CREAT, 0644, 1);
        } else {
            s = sem_open(name, O_CREAT, 0644, 0);
        }
        
        int pid, status, wstatus, swait;

        pid = fork();
        switch(pid) {
            case -1:
                perror("Fork error");
                exit(-1);
            case 0:
                sem_wait(s);
                execl("./process", id, name, sizeOfBelt, numElem, (char *) NULL);
                //perror("error in exec");
                break;
            default:
                wstatus = waitpid(pid,&status,0);
                if (wstatus > -1){
                    printf("[OK] [factory_manager] Process_manager with id %s has finished.\n", id);
                } else {
                    printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", id);
                }
                sem_close(s);
                sprintf(name, "/%d", index2+6);
                s=sem_open(name, O_RDWR);
                sem_post(s);
                
        }
        index2 = index2+6;
        
    }
    
    
    printf("Finishing\n");
    return 0;
    
}