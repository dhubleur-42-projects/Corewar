#ifndef VIRTUALMACHINE_H
# define VIRTUALMACHINE_H

# include <fcntl.h>

# include "libft.h"
# include "arguments.h"
# include "config.h"
# include "colors.h"

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
	int cycle;
	memory_t *memory;
	t_list *processes;
} virtualmachine_t;

bool init_virtualmachine(virtualmachine_t *vm);
bool load_champions(virtualmachine_t *vm, arguments_t *args);
void free_virtualmachine(virtualmachine_t *vm);
void dump_memory(const virtualmachine_t *vm, int fd);
void dump_processes(const virtualmachine_t *vm, int fd);

#endif