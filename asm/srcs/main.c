#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "corewar.h"
#include "exit_message.h"
#include "exit_status.h"
#include "libft.h"
#include "parse_file.h"
#include "safe_exit.h"
#include "utils.h"

static void parse_args(int ac, char **av, char const **champion_file_name);
static void die_usage();
static void print_usage();

char *PROG_NAME = NULL;

int main(int ac, char **av)
{
	char const *champion_file_name;

	parse_args(ac, av, &champion_file_name);
	parse_file(champion_file_name);
	//TEMP TODO compile + write to a file
}

static void parse_args(int ac, char **av, char const **champion_file_name)
{
	PROG_NAME = av[0];
	if (ac < 2)
		die_usage();
	if (ft_strlen(av[1]) > FILENAME_MAX_LEN)
		die(EXIT_MESSAGE_PROG_NAME_TOO_LONG, EXIT_STATUS_USAGE);
	if (!strend_with(av[1], ".s"))
		die_usage();
	*champion_file_name = av[1];
}

static void die_usage()
{
	print_usage();
	safe_exit(EXIT_STATUS_USAGE);
}

static void print_usage()
{
	if (PROG_NAME != NULL)
		ft_dprintf(2, "Usage: %s <champion.s>\n", PROG_NAME);
	else
		ft_dprintf(2, "Usage: ./asm <champion.s>\n");
}
