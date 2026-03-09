#ifndef COREWAR_H
# define COREWAR_H

# include <stdint.h>

# define PROG_MAX_LEN 128
# define FILENAME_MAX_LEN 130 /* PROG_MAX_LEN + strlen(".s") */
# define LINE_MAX_LEN 4096
# define PROG_NAME_LENGTH (128 + 4)
# define COMMENT_LENGTH (2048 + 4)
# define LABEL_CHAR ':'
# define LABEL_CHARS				"abcdefghijklmnopqrstuvwxyz_0123456789"
# define REG_NUMBER 16
# define COREWAR_EXEC_MAGIC {0x00, 0xea, 0x83, 0xf3}

# define REG_CODE 1
# define DIR_CODE 2
# define IND_CODE 3

# define IND_SIZE 2
# define REG_SIZE 4
# define DIR_SIZE REG_SIZE

# define REG_ARG_SIZE 1
# define DIR_ARG_SIZE 4
# define IND_ARG_SIZE 2

typedef struct s_header
{
	char *name;
	char *comment;
} t_header;

typedef enum e_token_type
{
	LABEL,
	INSTRUCTION,
} t_token_type;

typedef enum e_op
{
	ALIVE = 1,
	LOAD = 2,
	STORE = 3,
	ADDITION = 4,
	SUBSTRACTION = 5,
	AND = 6,
	OR = 7,
	XOR = 8,
	ZJMP = 9,
	LOAD_INDEX = 10,
	STORE_INDEX = 11,
	FORK = 12,
	LONG_LOAD = 13,
	LONG_LOAD_INDEX = 14,
	LONG_FORK = 15,
	AFF = 16,
} t_op;

typedef enum e_arg_type
{
	DIRECT_LABEL,
	DIRECT_NUMERIC,
	INDIRECT_LABEL,
	INDIRECT_NUMERIC,
	REG
} t_arg_type;

typedef struct s_arg
{
	t_arg_type arg_type;
	union
	{
		uint8_t reg_number;
		uint32_t numeric_value;
		char *label;
	};
} t_arg;

typedef struct s_instruction
{
	t_op op;
	t_arg args[4];
	uint8_t n_args;
	char *label;
} t_instruction;

typedef struct s_label_pos
{
	char const *label_ref;
	uint32_t pos;
} t_label_pos;

extern char *PROG_NAME;

#endif
