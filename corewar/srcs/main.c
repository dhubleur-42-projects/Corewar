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

	t_list *processes = vm.processes;
	while (processes != NULL) {
		process_t *process = (process_t *)processes->content;
		ft_dprintf(1, "Process %d: PC=0x%x, Carry=%d Registers:\n", process->id, process->pc, process->carry);
		for (int i = 0; i < REG_NUMBER; i++) {
			ft_dprintf(1, "  R%d: ", i + 1);
			for (int j = 0; j < REG_SIZE; j++) {
				ft_dprintf(1, "0x%x ", process->regs[i][j]);
			}
			ft_dprintf(1, "\n");
		}
		processes = processes->next;
	}

	free_virtualmachine(&vm);
	free_arguments(&args);
	return 0;
}
