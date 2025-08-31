#ifndef VIRTUALMACHINE_H
# define VIRTUALMACHINE_H

# include <fcntl.h>

# include "libft.h"
# include "arguments.h"
# include "config.h"
# include "colors.h"
# include "champion.h"
# include "utils.h"

typedef struct {
	int address;
	int writer;
	uint8_t value;
} cell_t;

typedef struct {
	int size;
	cell_t *cells;
} memory_t;

typedef struct {
    int cycles;
    size_t bytes_used;
    // Anything that is necessary to execute the instruction later (defined by B, opaque for A)
} instruction_t;

typedef struct {
	int id;
	int owner;
	size_t pc;
	uint8_t **regs;
	int carry;
	instruction_t *current_instruction;
} process_t;

typedef struct {
	int number_of_champions;
	champion_t *champions;
	int cycle;
	memory_t *memory;
	list_t *processes;
	int cycle_to_die;
	int last_check_cycle;
	int checks_since_decrease;
	int lives_since_check;
} virtualmachine_t;

bool init_virtualmachine(virtualmachine_t *vm);
bool load_champions(virtualmachine_t *vm, arguments_t *args);
void free_virtualmachine(virtualmachine_t *vm);
void dump_memory(const virtualmachine_t *vm, int fd);
void dump_processes(const virtualmachine_t *vm, int fd);

// Return true and fill instr if the given process is on a valid instruction
// Return false if the process is not on a valid instruction
// instr->cycles is filled with the cycle count of the valid instruction
// instr->bytes_uses is filled with the number of bytes of the valid instruction
bool is_valid_instruction(virtualmachine_t *vm, process_t *process, instruction_t *instr);

typedef struct {
    bool has_executed_live;
    int live_parameter;
    bool has_jumped;
} instruction_result_t;

// Execute the instruction inst for the given process
// Fill `has_executed_live`=true if it was a live instruction
// and in this case fill the `live_parameter` with the parameter used in the instruction
// Fill the has_jumped variable to know if we need to move the PC
void execute_instruction(virtualmachine_t *vm, process_t *process, instruction_t *instr, instruction_result_t *result);

bool is_simulation_finished(virtualmachine_t *vm, arguments_t *args);
bool do_cycle(virtualmachine_t *vm);

#endif