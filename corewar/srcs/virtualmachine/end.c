#include "virtualmachine.h"

void detect_winner(virtualmachine_t *vm) {
	int last_live = -1;
	int count = 0;

	for (int i = 0; i < vm->number_of_champions; i++) {
		if (vm->champions[i].last_live > last_live) {
			last_live = vm->champions[i].last_live;
			count = 1;
		} else if (vm->champions[i].last_live == last_live) {
			count++;
		}
	}

	if (count == 1) {
		ft_dprintf(1, "The winner is:\n");
	} else {
		ft_dprintf(1, "The winners are:\n");
	}

	for (int i = 0; i < vm->number_of_champions; i++) {
		if (vm->champions[i].last_live == last_live) {
			ft_dprintf(1, "\t- %s (champion %d) with last live at %d\n", vm->champions[i].name, vm->champions[i].number, vm->champions[i].last_live);
		}
	}
}