/*
 * main.c
 *
 *  Created on: 08.12.2012
 *      Author: ilya
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gl.h"
#include "parser.h"

// POSIX specific

int file_exists(char * filename) {
	struct stat	buffer;
	return ((stat (filename, &buffer) == 0) && S_ISREG(buffer.st_mode));
}


int main(int argc, char **argv) {
	char filename[1024];

	if (argc == 2)
		strcpy(filename, argv[1]);
	else
		strcpy(filename, "input.txt");


	if (file_exists(filename)) {
		parse(filename);
		start_gl_render(argc, argv);
	} else
		printf("Unable to open file %s\n", filename);
    return 0;
}

