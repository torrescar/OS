
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
   return (0);
}

