// after the program is run , the content of the created file can 
// be seen using the command hexdump -c file_hole

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
  int fd;
  char buf1[]="abc";
  
  if ( (fd= open("file_write1.txt", O_CREAT|O_RDONLY, 0666))<0)
  {
    perror("Error creating the file");
    exit(1);
  }


  if ( write(fd,buf1,3)< 0) {
    perror("write error");
    exit(1);		
  }

  if( lseek(fd,100,SEEK_CUR) < 0) {
     perror("seek error");
     exit(1);
  }


  if (close(fd)<0){    
    perror("close error");
    exit(1);
  }
 
  return 0;
}
