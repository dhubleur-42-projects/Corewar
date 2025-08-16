#include "virtualmachine.h"

#define COLOR(owner) \
	((owner) == -1 ? RESET : \
	((owner) == 1 ? GREEN : \
	((owner) == 2 ? BLUE : \
	((owner) == 3 ? YELLOW : \
	((owner) == 4 ? RED : \
	(RESET))))))

void dump_memory(const virtualmachine_t *vm, int fd) {
	if (vm == NULL || vm->memory == NULL || vm->memory->cells == NULL) {
		ft_dprintf(fd, "Memory is not initialized.\n");
		return;
	}
	for (size_t i = 0; i < MEM_SIZE; i++) {
		if (i % 32 == 0) {
			ft_dprintf(fd, "%s0x%s%x: ", 
				PURPLE,
				// TODO: cleaner 0 padding
				(i < 0x10 ? "00" : i < 0x100 ? "0" : ""),
				i);
		}
		ft_dprintf(fd, "%s%s%x%s", 
			COLOR(vm->memory->cells[i].owner),
			// TODO: cleaner 0 padding
			(vm->memory->cells[i].value < 0x10 ? "0" : ""),
			vm->memory->cells[i].value,
			RESET);
		if ((i + 1) % 32 == 0) {
			ft_dprintf(fd, "\n");
		} else {
			ft_dprintf(fd, " ");
		}
	}
}