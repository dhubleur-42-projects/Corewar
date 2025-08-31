#include "virtualmachine.h"

#include <stdlib.h>

//TMP
bool is_valid_instruction(virtualmachine_t *vm, process_t *process, instruction_t *instr) {
	(void)vm;
	(void)instr;
	process->current_instruction->cycles = rand() % 11 + 10;
	process->current_instruction->bytes_used = rand() % 5 + 5;
	return true;
}

//TMP
void execute_instruction(virtualmachine_t *vm, process_t *process, instruction_t *instr, instruction_result_t *result) {
	(void)vm;
	(void)process;
	(void)instr;
	int rand_val = rand() % 3 + 1;
	if (rand_val == 1) {
		result->has_jumped = true;
		process->pc = rand() % MEM_SIZE;
	} else {
		result->has_jumped = false;
		if (rand_val == 2) {
			result->has_executed_live = true;
			result->live_parameter = process->owner;
		}
	}
}


bool is_simulation_finished(virtualmachine_t *vm, arguments_t *args) {
	if (vm->cycle == args->dump_cycle) {
		return true;
	}
	if (vm->processes == NULL) {
		return true;
	}
	return false;
}

void clean_instruction(process_t *process) {
	if (process->current_instruction != NULL) {
		free(process->current_instruction);
		process->current_instruction = NULL;
	}
}

void progress_process(process_t *process) {
	process->pc = (process->pc + process->current_instruction->bytes_used) % MEM_SIZE;
	clean_instruction(process);
}

bool do_process_read(virtualmachine_t *vm, process_t *process) {
	if (process->current_instruction == NULL) {
		// DEBUG
		ft_dprintf(2, "Process %d, cycle %d: read instr\n", process->id, vm->cycle);
		process->current_instruction = malloc(sizeof(instruction_t));
		if (process->current_instruction == NULL) {
			ft_dprintf(2, "Error: Memory allocation failed.\n");
			return false;
		}
		if (!is_valid_instruction(vm, process, process->current_instruction)) {
			progress_process(process);
		}
		// DEBUG
		ft_dprintf(2, "\t <bytes_used: %d, cycles: %d>\n", process->current_instruction->bytes_used, process->current_instruction->cycles);
	}
	return true;
}

bool do_process_exec(virtualmachine_t *vm, process_t *process) {
	if (process->current_instruction != NULL) {
		if (process->current_instruction->cycles > 0) {
			process->current_instruction->cycles -= 1;
		} else {
			// DEBUG
			ft_dprintf(2, "Process %d, cycle %d: execute instr\n", process->id, vm->cycle);

			instruction_result_t result;
			execute_instruction(vm, process, process->current_instruction, &result);

			// DEBUG
			ft_dprintf(2, "\t <has_jumped: %d, has_executed_live: %d, live_parameter: %d>\n", result.has_jumped, result.has_executed_live, result.has_executed_live ? result.live_parameter : -1);

			if (result.has_jumped) {
				clean_instruction(process);
			} else {
				progress_process(process);
			}

			if (result.has_executed_live) {
				vm->lives_since_check += 1;
				champion_t *champion = NULL;
				for (int i = 0; i < vm->number_of_champions; i++) {
					if (vm->champions[i].number == result.live_parameter) {
						vm->champions[i].last_live = vm->cycle;
						champion = &vm->champions[i];
						break;
					}
				}
				ft_dprintf(1, "Champion %d (%s) has been declared alive by process %d\n", result.live_parameter, champion != NULL ? champion->name : "incorrect", process->id);
			}

			// DEBUG
			ft_dprintf(1, "\t PC now at %d\n", process->pc);
		}
	}
	return true;
}

bool do_cycle(virtualmachine_t *vm) {
	list_t *current_process = vm->processes;

	while (current_process != NULL) {
		if(!do_process_read(vm, (process_t *)current_process->content)) {
			return false;
		}
		current_process = current_process->next;
	}

	current_process = vm->processes;
	while (current_process != NULL) {
		if(!do_process_exec(vm, (process_t *)current_process->content)) {
			return false;
		}
		current_process = current_process->next;
	}

	// TODO: checks

	vm->cycle += 1;
	return true;
}