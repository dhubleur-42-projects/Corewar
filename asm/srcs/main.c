#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "compile_to_file.h"
#include "corewar.h"
#include "exit_message.h"
#include "exit_status.h"
#include "libft.h"
#include "parse_file.h"
#include "safe_alloc.h"
#include "safe_exit.h"
#include "utils.h"

static void parse_args(int ac, char **av, char const **champion_file_name);
static void die_usage();
static void print_usage();
static char *get_allocated_compiled_file_name(char const *champion_file_name);
static void safe_free_header(t_header *header);
static void lst_wrapper_safe_free_instruction(void *instruction);

char *PROG_NAME = NULL;

int main(int ac, char **av)
{
	char const *champion_file_name;
	char *compiled_file_name;
	t_header header;
	list_t *instructions = NULL;

	parse_args(ac, av, &champion_file_name);
	parse_file(champion_file_name, &header, &instructions);
	compiled_file_name = get_allocated_compiled_file_name(champion_file_name);
	compile_to_file(compiled_file_name, &header, instructions);

	safe_free(compiled_file_name);
	safe_free_header(&header);
	safe_lstclear(&instructions, &lst_wrapper_safe_free_instruction);
	safe_exit(0);
}

static void parse_args(int ac, char **av, char const **champion_file_name)
{
	PROG_NAME = av[0];
	if (ac < 2)
		die_usage();
	if (ft_strlen(av[1]) > FILENAME_MAX_LEN)
		die(EXIT_STATUS_USAGE, EXIT_MESSAGE_PROG_NAME_TOO_LONG);
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

static char *get_allocated_compiled_file_name(char const *champion_file_name)
{
	char *compiled_file_name;

	compiled_file_name = safe_malloc(ft_strlen(champion_file_name) - sizeof(".s") + sizeof(".cor") + 1);
	ft_strcpy(compiled_file_name, champion_file_name);	/* never fail */
	ft_strcpy(compiled_file_name + ft_strlen(champion_file_name) - sizeof(".s") + 1 /* NULL of sizeof */, ".cor"); /* never fail */

	return compiled_file_name;
}

static void safe_free_header(t_header *header)
{
	safe_free(header->name);
	safe_free(header->comment);
}

static void lst_wrapper_safe_free_instruction(void *instruction_void)
{
	t_instruction *instruction;

	instruction = (t_instruction *)instruction_void;
	for (size_t i = 0; i < instruction->n_args; i++)
	{
		if (instruction->args[i].arg_type == DIRECT_LABEL
		||  instruction->args[i].arg_type == INDIRECT_LABEL)
			safe_free(instruction->args[i].label);
	}
	safe_free(instruction->label);
	safe_free(instruction);
}
