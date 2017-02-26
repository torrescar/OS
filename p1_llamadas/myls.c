#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call gtcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>


int main(int argc, char *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	int fd;
	char buf[PATH_MAX];

	if(argc < 2)
	{
		if((dirp=opendir(getcwd(buf, PATH_MAX))) == NULL) {
			perror("couldn't open directory");
			return -1;
		}
	}
	else {
		if((dirp=opendir(argv[1])) == NULL) {
			perror("couldn't open directory");
			return -1;
		}
	}
	
	
	  while (dirp) {
   		 if ((dp = readdir(dirp)) != NULL) {
        		printf("%s\n",dp->d_name);
      		 } else {
         		closedir(dirp);
         		return -1;
      		 }
         }
 	 return 0;
}

