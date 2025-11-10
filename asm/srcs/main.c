#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "libft.h"

#define PROG_MAX_LEN 128
#define FILENAME_MAX_LEN 130 /* PROG_MAX_LEN + strlen(".s") */

#define EXIT_STATUS_USAGE 1

#define STR(X) STR2(X)
#define STR2(X) #X

char *PROG_NAME = NULL;

static void parse_args(int ac, char **av, char const **champion_file_name);
static bool strend_with(char const *str, char const *suffix);
static void die_usage();
static void print_usage();
static void die(char const *err, int status);

int main(int ac, char **av)
{
	char const *champion_file_name;

	parse_args(ac, av, &champion_file_name);
}

static void parse_args(int ac, char **av, char const **champion_file_name)
{
	PROG_NAME = av[0];
	if (ac < 2)
		die_usage();
	if (ft_strlen(av[1]) > FILENAME_MAX_LEN)
		die("Program name must not exceed "STR(FILENAME_MAX_LEN)" characters\n", EXIT_STATUS_USAGE);
	if (!strend_with(av[1], ".s"))
		die_usage();
	*champion_file_name = av[1];
}

static bool strend_with(char const *str, char const *suffix)
{
	unsigned int len = ft_strlen(str);
	unsigned int suffix_len = ft_strlen(suffix);
	unsigned int i;

	for (i = 0; i < suffix_len && i < len; i++)
		if (str[len - i - 1] != suffix[suffix_len - i - 1])
			return false;
	return i == suffix_len;
}

static void die_usage()
{
	print_usage();
	exit(EXIT_STATUS_USAGE);
}

static void print_usage()
{
	if (PROG_NAME != NULL)
		ft_dprintf(2, "Usage: %s <champion.s>\n", PROG_NAME);
	else
		ft_dprintf(2, "Usage: ./asm <champion.s>\n");
}

static void die(char const *err, int status)
{
	write(2, err, ft_strlen(err));
	exit(status);
}
