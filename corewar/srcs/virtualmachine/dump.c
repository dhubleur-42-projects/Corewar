#include "virtualmachine.h"

#define COLOR(writer) \
	((writer) == -1 ? RESET : \
	((writer) == 1 ? GREEN : \
	((writer) == 2 ? BLUE : \
	((writer) == 3 ? YELLOW : \
	((writer) == 4 ? RED : \
	(RESET))))))

int hex_len(unsigned int v) {
    int len = 0;
    if (v == 0) return 1;
    while (v > 0) {
        v /= 16;
        len++;
    }
    return len;
}

const char *get_hexa_zero_padding(int value, int size) {
	static char padding[10];
	int len = size - hex_len(value);
	for (int i = 0; i < len; i++) {
		padding[i] = '0';
	}
	padding[len] = '\0';
	return padding;
}

void dump_memory(const virtualmachine_t *vm, int fd) {
	if (vm == NULL || vm->memory == NULL || vm->memory->cells == NULL) {
		ft_dprintf(fd, "Memory is not initialized.\n");
		return;
	}
	for (size_t i = 0; i < MEM_SIZE; i++) {
		if (i % 32 == 0) {
			ft_dprintf(fd, "%s0x%s%x: ", 
				PURPLE,
				get_hexa_zero_padding(i, MEM_SIZE < 0x10000 ? 4 : 8),
				i);
		}
		ft_dprintf(fd, "%s%s%x%s", 
			COLOR(vm->memory->cells[i].writer),
			get_hexa_zero_padding(vm->memory->cells[i].value, 2),
			vm->memory->cells[i].value,
			RESET);
		if ((i + 1) % 32 == 0) {
			ft_dprintf(fd, "\n");
		} else {
			ft_dprintf(fd, " ");
		}
	}
}

void dump_processes(const virtualmachine_t *vm, int fd) {
	t_list *processes = vm->processes;
	while (processes != NULL) {
		process_t *process = (process_t *)processes->content;
		champion_t *champion = NULL;
		for (int i = 0; i < vm->number_of_champions; i++) {
			if (vm->champions[i].number == process->owner) {
				champion = &vm->champions[i];
				break;
			}
		}
		if (champion == NULL) {
			ft_dprintf(fd, "Error: Champion for process %d not found.\n", process->id);
			continue;
		}
		ft_dprintf(fd, "Process %d: CHAMPION=%s(%i) PC=0x%s%x, Carry=%d Registers:\n", process->id, champion->name, champion->number,get_hexa_zero_padding(process->pc, 4), process->pc, process->carry);
		for (int i = 0; i < REG_NUMBER; i++) {
			ft_dprintf(fd, "  R%d: ", i + 1);
			for (int j = 0; j < REG_SIZE; j++) {
				ft_dprintf(fd, "0x%s%x ", get_hexa_zero_padding(process->regs[i][j], 2), process->regs[i][j]);
			}
			ft_dprintf(fd, "\n");
		}
		processes = processes->next;
	}
}