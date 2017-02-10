
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

  DIR *dirp;
  struct dirent *dp;
  int fd;

  if ((dirp = opendir("dir_new")) == NULL) {
    if ( mkdir("dir_new", 0755)<0)
      {
	perror("Error creating directory");
	exit(1);
      }

    if ((dirp = opendir("dir_new")) == NULL) {
      perror("couldn't open directory");
      return -1;
    }
  }
  
  // Insert your code here
  if ( (fd=creat("file_writeread.txt",0666))<0){
    perror("Error creating the file");
    exit(1);
  }

  if (close(fd)<0){    
    perror("close error");
    exit(1);
  }
  
  while (dirp) {
    if ((dp = readdir(dirp)) != NULL) {
      if (strcmp(dp->d_name, "file_writeread.txt") == 0) {
        closedir(dirp);
        return 0;
      } else {
         closedir(dirp);
         return -1;
      }
    } 
  }
   return (0);
}

