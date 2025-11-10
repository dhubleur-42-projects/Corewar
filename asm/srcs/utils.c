#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define EXIT_STATUS_IO 5

int try_open(char const *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("An error occured while opening file");
		exit(EXIT_STATUS_IO);
	}
	return fd;
}
