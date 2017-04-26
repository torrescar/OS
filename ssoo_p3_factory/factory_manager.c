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
    int maxNumPM, idPM, sizeBelt, numOfElem;
    int fd,n;
    
    /*If the number of arguments is less than two, we print an error message and return -1.*/
    if(argc<2){
        perror("[ERROR][factory_manager] Invalid file1\n");
        return -1;
    }
    
    /*If the value of the file descriptor returned from opening the given file is less than 0, we print an error message and return -1.*/
    if ((fd=open(argv[1],O_RDONLY,0666))<0) {
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
        char name[100];
        sprintf(name, "/%d", index2);
        sem_t *s;
        if(index2==2){
            s = sem_open(name, O_CREAT, 0644, 1);
        } else {
            s = sem_open(name, O_CREAT, 0644, 0);
        }
        
        int pid, status;

        pid = fork();
        switch(pid) {
            case -1:
                perror("Fork error");
                exit(-1);
            case 0:
                //printf("pid %d", getpid());
                printf("here instead");
                /*char **argv;
                argv[0] = ("./process");
                argv[1] =&(buf[index2]);
                argv[2] =(name);
                argv[3] =&(buf[index2+2]);
                argv[4] =&(buf[index2+4]);
                argv[5]= NULL;
                
                
                //printf("%c", cIndex2);
                //printf("\n");
                
                //int resultExec = (execv("process", argv));
                //printf("[OK][factory_manager] Process_manager with id <id> has been created.");
                
                if (resultExec ==-1){
                    perror("[ERROR][factory_manager] Process_manager with id <id> has finished with errors.");
                    exit(-1);
                } else {
                    printf("[OK][factory_manager] Process_manager with id <id> has finished.");
                    sprintf(name, "/%d", index2+6);
                    s=sem_open(name, O_EXCL, 0644, 0);
                    sem_post(s);
                    exit(0);
                }*/
                //printf("my val %d",atoi(&buf[index2]));
                execl("./process", &buf[index2], name, &buf[index2+2], &buf[index2+4], (char *) NULL);
                //perror("error in exec");
                break;
            default:
                if (waitpid(pid,&status,0) > -1){
                    printf("[OK][factory_manager] Process_manager with id %c has finished.\n", buf[index2]);
                } else {
                    printf("[ERROR][factory_manager] Process_manager with id %c has finished with errors.\n", buf[index2]);
                }
                sprintf(name, "/%d", index2+6);
                s=sem_open(name, O_EXCL, 0644, 0);
                sem_post(s);
        }
        index2 = index2+6;
        
    }	
    
    printf("Finishing\n");
    return 0;
    
}