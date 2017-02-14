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

	/*If the number of arguments is less than two (argv[0] -> program, argv[|] -> file to be shown) we print the error and return -1*/
	if(argc < 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}
        if ((fd=open(argv[1],O_RDONLY,0666))<0)
	{
    		perror("Error opening file");
  		return -1;
	}
        while ((n=read(fd,buf,1))>0)
  	{
		write(STDOUT_FILENO, buf, 1);
	}
	if (n<0) {
    		perror("Read error occured");
    		return -1;
  	}
	close(fd);

	return 0;
}

