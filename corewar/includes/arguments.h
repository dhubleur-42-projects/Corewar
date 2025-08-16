#ifndef ARGUMENTS_H
# define ARGUMENTS_H

# include <stdbool.h>

# include "config.h"
# include "libft.h"

typedef struct {
	int number;
	char *filename;
} champion_t;

typedef struct {
	int dump_cycle;
	int number_of_champions;
	champion_t *champions;
} arguments_t;

typedef enum {
	CHAMPION,
	DUMP,
	NUMBER,
} parsing_state_t;

bool parse_arguments(int argc, char **argv, arguments_t *args);

void free_arguments(arguments_t *args);

#endif