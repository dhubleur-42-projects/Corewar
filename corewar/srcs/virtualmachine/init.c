#include "virtualmachine.h"

void free_process(void *ptr) {
	process_t *process = (process_t *)ptr;
	if (process != NULL) {
		if (process->regs != NULL) {
			for (int i = 0; i < REG_NUMBER; i++) {
				free(process->regs[i]);
			}
			free(process->regs);
		}
		free(process);
	}
}

void free_virtualmachine(virtualmachine_t *vm) {
	if (vm->memory != NULL) {
		if (vm->memory->cells != NULL) {
			free(vm->memory->cells);
		}
		free(vm->memory);
	}
	if (vm->processes != NULL) {
		ft_lstclear(&vm->processes, free_process);
	}
}

bool init_virtualmachine(virtualmachine_t *vm) {
	vm->cycle = 0;
	vm->memory = NULL;
	vm->processes = NULL;

	vm->memory = malloc(sizeof(memory_t));
	if (vm->memory == NULL) {
		ft_dprintf(2, "Error: Memory allocation failed.\n");
		return false;
	}

	vm->memory->size = MEM_SIZE;
	vm->memory->cells = malloc(MEM_SIZE * sizeof(cell_t));
	if (vm->memory->cells == NULL) {
		ft_dprintf(2, "Error: Memory allocation failed.\n");
		return false;
	}

	for (size_t i = 0; i < MEM_SIZE; i++) {
		vm->memory->cells[i].address = i;
		vm->memory->cells[i].writer = -1;
		vm->memory->cells[i].value = 0;
	}

	return true;
}

bool load_champions(virtualmachine_t *vm, arguments_t *args) {
	int space_between_entries = MEM_SIZE / args->number_of_champions;
	for (int i = 0; i < args->number_of_champions; i++) {
		champion_t *champion = &args->champions[i];

		int fd = open(champion->filename, O_RDONLY);
		if (fd < 0) {
			ft_dprintf(2, "Error: Could not open champion file %s.\n", champion->filename);
			return false;
		}

		uint8_t buffer[CHAMP_MAX_SIZE];
		size_t bytes_read = read(fd, buffer, CHAMP_MAX_SIZE);
		close(fd);
		if (bytes_read < 0) {
			ft_dprintf(2, "Error: Could not read champion file %s.\n", champion->filename);
			return false;
		}
		if (bytes_read == 0) {
			ft_dprintf(2, "Error: Champion file %s is empty.\n", champion->filename);
			return false;
		}
		if (bytes_read > CHAMP_MAX_SIZE) {
			ft_dprintf(2, "Error: Champion file %s is too large.\n", champion->filename);
			return false;
		}

		int start_address = i * space_between_entries;

		if (start_address + bytes_read > MEM_SIZE) {
			ft_dprintf(2, "Error: Champion %s exceeds memory bounds.\n", champion->filename);
			return false;
		}

		for (size_t j = 0; j < bytes_read; j++) {
			vm->memory->cells[start_address + j].value = buffer[j];
			vm->memory->cells[start_address + j].writer = champion->number;
		}

		process_t *process = malloc(sizeof(process_t));
		if (process == NULL) {
			ft_dprintf(2, "Error: Memory allocation failed.\n");
			return false;
		}

		process->id = champion->number;
		process->owner = champion->number;
		process->pc = start_address;
		process->carry = 0;
		process->regs = malloc(REG_NUMBER * sizeof(uint8_t *));
		if (process->regs == NULL) {
			ft_dprintf(2, "Error: Memory allocation failed.\n");
			free(process);
			return false;
		}
		for (int j = 0; j < REG_NUMBER; j++) {
			process->regs[j] = malloc(REG_SIZE * sizeof(uint8_t));
			if (process->regs[j] == NULL) {
				ft_dprintf(2, "Error: Memory allocation failed.\n");
				for (int k = 0; k < j; k++) {
					free(process->regs[k]);
				}
				free(process->regs);
				free(process);
				return false;
			}
			if (j == 0) {
				process->regs[j][REG_SIZE - 1] = champion->number;
			}
		}

		void *new_process = ft_lstnew(process);
		if (new_process == NULL) {
			ft_dprintf(2, "Error: Memory allocation failed.\n");
			free_process(process);
			return false;
		}
		ft_lstadd_back(&vm->processes, new_process);
	}
	return true;
}