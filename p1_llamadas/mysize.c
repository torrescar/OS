#include <stdio.h>	/*Header file for system call printf*/
#include <sys/types.h>	/*Header file for system call fstat*/
#include <sys/stat.h>
#include <fcntl.h>	/*Header file for system call fcntl*/
#include <dirent.h> 	/*Header file for system call opendir, closedir,readdir...*/
#include <unistd.h>


/*The program obtains the current directory and prints all the regular files and their sizes in bytes.*/
int main(int argc, char *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	int fd;
	char buf[PATH_MAX];
	int fileSize;

	
	/*The program tries to open the current directory and set the directory pointer dirp to a pointer for the directory stream of the current directory. If the current directory cannot be opened, an error message is printed and -1 is returned.*/
	if((dirp=opendir(getcwd(buf, PATH_MAX))) == NULL) {
		perror("couldn't open directory");
		return -1;
	}
	
	
	/*The program attempts to read each entry in the current directory and set it to the direct pointer, dp. If the program is unable to read the entry, the directory is closed and -1 is returned. */
    int success = -1;
	while ((dp = readdir(dirp)) != NULL) {
        success = 0;
	
	/*If the entry is not a regular file, the program jumps directly to the start of the loop and moves on to the next entry in the directory.*/
        if (dp->d_type != DT_REG)
            continue;
	
	/*If opening the regular file results in a negative integer, an error message is printed and -1 is returned.*/
        if ( (fd= open(dp->d_name, O_RDONLY, 0666))<0) {
            perror("Error opening the file");
            return -1;
        }
        
	/*filesize is set to the output of the function lseek, which returns the size of the file. SEEK_END sets the offset to the size of the file plus offset bytes and in this case the offset bytes is 0. If lseek results in a negative integer, an error message is printed and -1 returned. Otherwise, the file is closed and the file name and file size are printed.*/
        if((fileSize=lseek(fd,0,SEEK_END)) < 0) {
            perror("seek error");
            return -1;
        }
        close(fd);
        printf("%s\t%d\n",dp->d_name,fileSize);
	}
	
	
    /*The directory is closed and 0 is returned if the program was able to read the entries in the directory. If the program was not able to read the entries, it did not enter the while loop and the variable success never got set to 0. Thus, -1 is returned.*/
    closedir(dirp);
 	return success;
}
