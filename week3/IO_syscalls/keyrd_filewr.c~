#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
  
  int n,fd;
  char buff[24];

// creates a new file with  permitions 666
// fd= file descriptor  
  if ( (fd=creat("file_write2.txt",0666))<0)
  {
    perror("error creating the file");
    exit(1);
  }
  
  while ( (n=read(0,buff,sizeof(buff))) > 0)
      write(fd,buff,n);

// if read returned -1 display an error message    
  if (n<0)
    perror("read error");
  
// close the file
  close(fd);

  return 0;
}
