#include "arguments.h"
#include "virtualmachine.h"

int main(int argc, char **argv)
{
	arguments_t args;

	if (!parse_arguments(argc, argv, &args)) {
		free_arguments(&args);
		return 1;
	}

	ft_dprintf(1, "Dump cycle: %d\n", args.dump_cycle);
	ft_dprintf(1, "Number of champions: %d\n", args.number_of_champions);
	for (int i = 0; i < args.number_of_champions; i++) {
		ft_dprintf(1, "Champion %d: %s\n", args.champions[i].number, args.champions[i].filename);
	}

	virtualmachine_t vm;
	if (!init_virtualmachine(&vm)) {
		free_virtualmachine(&vm);
		free_arguments(&args);
		return 1;
	}

	if (!load_champions(&vm, &args)) {
		free_virtualmachine(&vm);
		free_arguments(&args);
		return 1;
	}

	dump_memory(&vm, 1);
	dump_processes(&vm, 1);

	free_virtualmachine(&vm);
	free_arguments(&args);
	return 0;
}
