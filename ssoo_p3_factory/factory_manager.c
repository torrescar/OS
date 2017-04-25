/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>

int main (int argc, const char * argv[] ){

    int fd;
    
    fd = open(argv[1], O_RDONLY);
	return 0;
}
