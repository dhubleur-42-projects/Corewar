#include "arguments.h"

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

	free_arguments(&args);
	return 0;
}
