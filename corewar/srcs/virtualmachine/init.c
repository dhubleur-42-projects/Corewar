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
	if (vm->champions != NULL) {
		free(vm->champions);
	}
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
	vm->champions = NULL;
	vm->cycle = 0;
	vm->memory = NULL;
	vm->processes = NULL;
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->last_check_cycle = 0;
	vm->checks_since_decrease = 0;

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
	vm->number_of_champions = args->number_of_champions;
	vm->champions = malloc(args->number_of_champions * sizeof(champion_t));
	if (vm->champions == NULL) {
		ft_dprintf(2, "Error: Memory allocation failed.\n");
		return false;
	}

	int space_between_entries = MEM_SIZE / args->number_of_champions;
	for (int i = 0; i < args->number_of_champions; i++) {
		champion_argument_t *champion_arg = &args->champions[i];
		champion_t *champion = &vm->champions[i];

		int fd = open(champion_arg->filename, O_RDONLY);
		if (fd < 0) {
			ft_dprintf(2, "Error: Could not open champion file %s.\n", champion_arg->filename);
			return false;
		}

		size_t read_size = CHAMP_MAX_SIZE + sizeof(champion_header_t);

		uint8_t buffer[read_size];
		size_t bytes_read = read(fd, buffer, read_size);
		close(fd);
		if (bytes_read < 0) {
			ft_dprintf(2, "Error: Could not read champion file %s.\n", champion_arg->filename);
			return false;
		}
		if (bytes_read < sizeof(champion_header_t) + 1) {
			ft_dprintf(2, "Error: Champion file %s is too small.\n", champion_arg->filename);
			return false;
		}

		champion_header_t *header = (champion_header_t *)buffer;
		header->magic = big_to_little_endian(header->magic);
		header->prog_size = big_to_little_endian(header->prog_size);

		if (header->magic != COREWAR_EXEC_MAGIC) {
			ft_dprintf(2, "Error: Champion file %s is invalid.\n", champion_arg->filename);
			return false;
		}
		if (header->prog_size > CHAMP_MAX_SIZE) {
			ft_dprintf(2, "Error: Champion file %s is too large (max size is %d bytes).\n", champion_arg->filename, CHAMP_MAX_SIZE);
			return false;
		}
		champion->number = champion_arg->number;
		ft_strcpy(champion->name, header->prog_name);

		int start_address = i * space_between_entries;
		size_t i = 0;
		for (size_t j = sizeof(champion_header_t); j < sizeof(champion_header_t) + header->prog_size; j++) {
			vm->memory->cells[start_address + i].value = buffer[j];
			vm->memory->cells[start_address + i].writer = champion_arg->number;
			i++;
		}

		process_t *process = malloc(sizeof(process_t));
		if (process == NULL) {
			ft_dprintf(2, "Error: Memory allocation failed.\n");
			return false;
		}

		process->id = champion_arg->number;
		process->owner = champion_arg->number;
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
				process->regs[j][REG_SIZE - 1] = champion_arg->number;
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