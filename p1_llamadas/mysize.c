#include <stdio.h>	/*Header file for system call printf*/
#include <sys/types.h>	/*Header file for system call fstat*/
#include <sys/stat.h>
#include <fcntl.h>	/*Header file for system call fcntl*/
#include <dirent.h> 	/*Header file for system call opendir, closedir,readdir...*/
#include <unistd.h>


int main(int argc, char *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	int fd;
	char buf[PATH_MAX];
	off_t fileSize;

	if((dirp=opendir(getcwd(buf, PATH_MAX))) == NULL) {
		perror("couldn't open directory");
		return -1;
	}
	
	while (dirp) {
   		if ((dp = readdir(dirp)) != NULL){  
			if (dp->d_type != DT_REG)
				continue;		
			if ( (fd= open(dp->d_name, O_RDONLY, 0666))<0)
 		  	{
    				perror("Error creating the file");
    				return -1;
  			}
			

		        if((fileSize=lseek(fd,0,SEEK_END)) < ((off_t) 0)) {

     				perror("seek error");
     				return -1;
  			}
        		printf("%s\t%d\n",dp->d_name,fileSize);
			
      		} else {
         		closedir(dirp);
         		return -1;
      		}
	}
 	return 0;
}
