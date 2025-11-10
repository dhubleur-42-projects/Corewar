#ifndef COREWAR_H
# define COREWAR_H

# define PROG_MAX_LEN 128
# define FILENAME_MAX_LEN 130 /* PROG_MAX_LEN + strlen(".s") */
# define LINE_MAX_LEN 4096
# define PROG_NAME_LENGTH 128
# define COMMENT_LENGTH 2048

typedef struct s_header
{
	char *name;
	char *comment;
} t_header;

extern char *PROG_NAME;

#endif
