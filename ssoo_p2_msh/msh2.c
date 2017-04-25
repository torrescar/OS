/*-
 * msh.c
 *
 * Minishell C source
 * Show how to use "obtain_order" input interface function
 *
 * THIS FILE IS TO BE MODIFIED
 */

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

extern int obtain_order();		/* See parser.y for description */

int main(void)
{
    char ***argvv;
    int command_counter;
    int num_commands;
    int args_counter;
    char *filev[3];
    int bg;
    int ret;
    
    setbuf(stdout, NULL);			/* Unbuffered */
    setbuf(stdin, NULL);
    
    while (1)
    {
        fprintf(stderr, "%s", "msh> "); /* Prompt */
        ret = obtain_order(&argvv, filev, &bg);
        if (ret == 0) {
            break;		/* EOF */
        }
        if (ret == -1) continue;	/* Syntax error */
        num_commands = ret - 1;		/* Line */
        if (num_commands == 0) continue;	/* Empty line */
        
        /*
         * THE PART THAT MUST BE REMOVED STARTS HERE
         * THE FOLLOWING LINES ONLY GIVE AN IDEA OF HOW TO USE THE STRUCTURES
         * argvv AND filev. THESE LINES MUST BE REMOVED.
         */
        
        /* Checks if command given is an internal command: either mycalc or mybak and executes accordingly */
        if (!strcmp(argvv[0][0], "mycalc")) {
            int n = sizeof(argvv[0]);
            mycalc(argvv[0][1], argvv[0][2], argvv[0][3]);
            continue;
        }
        else if (!strcmp(argvv[0][0], "mybak")) {
            printf("mybak\n");
            continue;
        }
        
        /* Otherwise, we know the command / command sequence is non-internal and we prepare to execute it through children of the minishell process by creating pipes:
         */
        
        // Creates a pipe for use between potential 1st and 2nd command
        int prev[2];
        pipe(prev);
        
        // Creates a pipe for use between potential 2nd and 3rd command
        int seq[2];
        pipe(seq);
        
        for (command_counter = 0; command_counter < num_commands; command_counter++)
        {
            
            int pid;
            int status;
            int fd0, fd1, fd2;
            int s;
            
            // Creates a child process spawned off the minishell such that each command in a command sequence is executed by its own child process of the minishell
            pid = fork();
            
            switch(pid) {
                case -1: // Reached if fork produced an error (in which case pid = -1 because fork() would've returned -1
                    perror("Error in fork");
                    exit(-1);
                case 0: // Reached if currently the child process is executing, because fork() returns 0 to the child
                    
                    /* Redirection of input: */
                    // 1) If first command in sequence, input gets redirected from an input file if specified, otherwise will just keep standard input.  To redirect, closes standard input file and duplicates the reference at the specified file descriptor to standard input's reference.
                    if (command_counter == 0) {
                        if (filev[0] != NULL){
                            close(0);
                            fd0 = open(filev[0], O_RDONLY);
                            dup(fd0);
                        }
                    }
                    // 2) If 2nd command in the sequence, input gets redirected from a pipe linked to the output of the 1st command in the sequence.  To redirect, closes standard input file and duplicates the reference at the file descriptor of the pipe read end to standard input's reference.
                    else if (command_counter == 1) {
                        close(0);
                        dup(prev[0]);
                    }
                    // 3) If 3rd command in the sequence, input gets redirected from a pipe linked to the output of the 2nd command in the sequence.  To redirect, closes standard input file and duplicates the reference at the file descriptor of the pipe read end to standard input's reference.
                    else {
                        close(0);
                        dup(seq[0]);
                    }
                    
                    
                    /* Redirection of output: */
                    // 1) If last command in sequence, output gets redirected to an output file if specified, otherwise will just keep standard output.  To redirect, closes standard output file and duplicates the reference at the specified file descriptor to standard output's reference.
                    if (command_counter == num_commands - 1) {
                        if (filev[1] != NULL){
                            close(1);
                            fd1 = open(filev[1], O_CREAT | O_WRONLY, 0666);
                            dup(fd1);
                        }
                        close(prev[0]);
                    }
                    // 2) If 1st command in the sequence, output gets redirected to a pipe linked to the input to be read by the 2nd command in the sequence.  To redirect, closes standard output file and duplicates the reference at the file descriptor of the pipe write end to standard output's reference.
                    else if (command_counter == 0) {
                        close(prev[0]);
                        close(1);
                        dup(prev[1]);
                    }
                    // 3) If 2nd command in the sequence, output gets redirected to a pipe linked to the input to be read by the 3rd command in the sequence.  To redirect, closes standard output file and duplicates the reference at the file descriptor of the pipe write end to standard output's reference.
                    else {
                        close(seq[0]);
                        close(1);
                        dup(seq[1]);
                    }
                    
                    /* Redirection of error: */
                    // If error file specified, standard error gets closed and error output gets redirected to the file specified by duplicating the reference at the specified file descriptor to the standard error's reference.
                    if (filev[2] != NULL){ // ERR
                        close(2);
                        fd2 = open(filev[2], O_CREAT| O_WRONLY, 0666);
                        dup(fd2);
                    }
                    
                    // Now that all input / output has been redirected appropriately, the command gets executed where the command name is referenced by argvv[command_counter][0], and all the arguments for that command are referenced by argvv[command_counter].  If and only if command is not properly execute, perror will execute and print the error from the execution.
                    execvp(argvv[command_counter][0], argvv[command_counter]);
                    perror("Error in exec");
                    break;
                    
                default: // Reached if currently the parent minishell process is executing, because fork() returns a non-zero integer representing the child PID
                    
                    /* If background command */
                    if (bg) {
                        waitpid(pid, &status, WNOHANG); // If background, parent waits for the child (to kill it appropriately when finished so no zombie processes are left), but without hanging so that the minishell still functions immediately after background command is requested
                        if (command_counter == num_commands -1) {
                            printf("[%d]\n", pid);
                        }
                    } else { // If not background, parent waits for the child (to kill it appropriately when finished so no zombie processes are left), and is blocked until child finishes execution.
                        while(wait(&status) != pid);
                    }
                    // Parent closes all write ends of pipes used by its children so that sequential children can read from the read ends of such pipes
                    close(prev[1]);
                    if (command_counter == 1) {
                        close(seq[1]);
                    }
                    
            }
            
        }
        
        
        /*
         * END OF THE PART TO BE REMOVED
         */
        
    } //fin while
    return 0;
    
} //end main

int mycalc(char op1[], char operator[], char op2[]){
    int sum, remainder, diff, quotient, Acc =0;
    //setenv("Acc",0,1);
    
    
    /*If the second parameters is equal to "add", add the first and third parameter and store in a sum variable and the environmental variable Acc.*/
    if(strcmp(operator, "add")==0){
        sum= atoi(op1) + atoi(op2);
        //Acc= *getenv(&Acc) + sum;
        printf("%d\n", sum);
        return 0;
    }
    /*If the second parameter is equal to "mod," calculate the modulo using parameter 1 as the dividend and parameter 2 as the divisor.*/
    else if(strcmp(operator, "mod")==0){
        remainder = atoi(op1) % atoi(op2);
        diff = atoi(op1) - remainder;
        quotient = diff / atoi(op2);
        printf("%d\n", remainder);
        return 0;
    }
    /*Otherwise print an error message and return -1. */
    else {
        perror("[ERROR] The structure of the command is <operand 1> <add/mod> <operand 2>");
        return -1;
    }
}
 
 int mybak(int argc, int file, int directory) {
     char buf[1024];
     int fd, fd2, n;
     
     /*If there are not 2 parameters, print an error message.*/
     if(argc != 2) {
         printf("[ERROR] The structure of the command is mybak <original file> <output directory>");
         return -1;
     }
     
     /*If the value of the file descriptor returned from opening the given file is a negative integer, we print an error message and return -1 */
     if ((fd=open("path",O_RDONLY,0666))<0){
         perror("Error opening original file");
         return -1;
     }
     
     /*If the value of the file descriptor returned from opening the new file is a negative integer, we print an error message and return -1 */
     if ((fd2=open("path",O_RDONLY,0666))<0){
         perror("[ERROR] Error opening the copied file");
         return -1;
     }
     
     
     /* The program will read the contents of the file into the immediate buffer, buf, of 1024 bytes. Standard output is closed and the contents of the buffer will be written to the new file.*/
     close(0);
     while ((n=read(fd,buf,1024))>0){
         write(fd2, buf, n);
     }
     
     /*If read is unsuccessful, n will have been set to -1, and the loop will be exited out of.  So, we print an error message and return -1*/
     if (n<0) {
         perror("[ERROR] Error opening the copied file");
         return -1;
     } 
     
     /*Otherwise, we print a message that the file has been copied successfully to the new directory and return 0.*/
     else {
         printf("[OK] Copy has been successful between <original filename> to the directory <output directory>.");
         return 0;
     }
 }