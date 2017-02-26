#include <stdio.h>		/* Header file for printf */
#include <sys/types.h>		/* Header file for system call  open */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>		/* Header file for system calls read, write y close */

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int fd, n;

	/*If the number of arguments is less than two (argv[0] -> program, argv[|] -> file to be shown) we print the error and return -1 (i.e. returns -1 if no argument was passed)*/
	if(argc < 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}
    
    /*If value of the file descriptor returned from opening the given file (passed in as an argument when main is run) is a negative integer, we print the error and return -1 (i.e. returns -1 if there was an error opening the file)*/
    if ((fd=open(argv[1],O_RDONLY,0666))<0)
	{
        perror("Error opening file");
  		return -1;
	}
    
    /*If there is no error getting a file to open, nor any opening that file, we will read the contents of the file into our immediate buffer, buf, of 1024 bytes.  The contents of the buffer will be written out to standard output*/
    while ((n=read(fd,buf,BUFFER_SIZE))>0)
  	{
		write(STDOUT_FILENO, buf, n);
	}
    
     /*If read is unsuccessful, n will have been set to -1, and the loop will be exited out of.  So, we print the error and return -1*/
	if (n<0) {
    		perror("Read error occured");
    		return -1;
  	}
    
    /*Otherwise, we close the file descriptor, and acknowledge reading and writing success by returning 0.*/
	close(fd);

	return 0;
}

