#include "virtualmachine.h"

#define COLOR(owner) \
	((owner) == -1 ? RESET : \
	((owner) == 1 ? GREEN : \
	((owner) == 2 ? BLUE : \
	((owner) == 3 ? YELLOW : \
	((owner) == 4 ? RED : \
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
			COLOR(vm->memory->cells[i].owner),
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