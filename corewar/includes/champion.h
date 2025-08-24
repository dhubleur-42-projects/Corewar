#ifndef CHAMPION_H
# define CHAMPION_H

# include "config.h"

# define PROG_NAME_LENGTH		(128)
# define COMMENT_LENGTH			(2048)
# define COREWAR_EXEC_MAGIC		0xea83f3

typedef struct
{
	uint32_t	magic;
	char	prog_name[PROG_NAME_LENGTH + 1];
	uint32_t	prog_size;
	char	comment[COMMENT_LENGTH + 1];
}	champion_header_t;

typedef struct {
	int number;
	char name[PROG_NAME_LENGTH + 1];
} champion_t;

#endif