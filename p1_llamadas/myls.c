#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call gtcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>


int main(int argc, char *argv[])
{
	DIR *dirp;
	struct dirent *dp;
	char buf[PATH_MAX];

    /* If no arguments are passed when main is run, program tries to open the current directory for reading and sets the directory pointer, dirp, to the pointer to the directory stream for current directory.  However, if current directory cannot be opened, its contents cannot be read, so it prints the opening error and program returns -1 */
	if(argc < 2)
	{
		if((dirp=opendir(getcwd(buf, PATH_MAX))) == NULL) {
			perror("couldn't open directory");
			return -1;
		}
	}
    /* If an arguments IS specified when main is run, program tries to open the first directory specified for reading and sets the directory pointer, dirp, to the pointer to the directory stream for that specified directory.  However, if the directory specified cannot be opened (including if it is not accessible or not actually a directory in existence or in type), its contents cannot be read, so it prints the opening error and program returns -1 */
	else {
		if((dirp=opendir(argv[1])) == NULL) {
			perror("couldn't open directory");
			return -1;
		}
	}
	
    /* At this point, there is definitely a directory specified that can be opened, so program tries to read it (referenced by dirp).  If program cannot read its entries, there is no next directory entry to be pointed to by dirent pointer, dp, so dp is set to NULL and the directory stream, dirp, is closed and program returns -1 because success is never set to 0 if readdir(dirp) never worked.  Otherwise, 0 is returned because dp at some point had a next entry to list since readdir(dirp) worked */
    int success = -1;
    while ((dp = readdir(dirp)) != NULL) {
        success = 0;
        printf("%s\n",dp->d_name);
    }
    
    closedir(dirp);
    return success;
}

