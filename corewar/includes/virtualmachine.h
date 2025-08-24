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
	int id;
	int owner;
	size_t pc;
	uint8_t **regs;
	int carry;
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
} virtualmachine_t;

bool init_virtualmachine(virtualmachine_t *vm);
bool load_champions(virtualmachine_t *vm, arguments_t *args);
void free_virtualmachine(virtualmachine_t *vm);
void dump_memory(const virtualmachine_t *vm, int fd);
void dump_processes(const virtualmachine_t *vm, int fd);

#endif