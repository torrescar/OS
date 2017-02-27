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
	int fileSize;

	if((dirp=opendir(getcwd(buf, PATH_MAX))) == NULL) {
		perror("couldn't open directory");
		return -1;
	}
	
    int success = -1;
	while ((dp = readdir(dirp)) != NULL) {
        success = 0;
        if (dp->d_type != DT_REG)
            continue;
        if ( (fd= open(dp->d_name, O_RDONLY, 0666))<0) {
            perror("Error opening the file");
            return -1;
        }
        
        if((fileSize=lseek(fd,0,SEEK_END)) < 0) {
            perror("seek error");
            return -1;
        }
        close(fd);
        printf("%s\t%d\n",dp->d_name,fileSize);

	}
    closedir(dirp);
 	return success;
}
