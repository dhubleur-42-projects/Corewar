#include <limits.h>
#include <stdint.h>

#include "corewar.h"
#include "exit_message.h"
#include "exit_status.h"
#include "fcntl.h"
#include "libft.h"
#include "safe_alloc.h"
#include "safe_file.h"
#include "utils.h"

#include "compile_to_file.h"


static void write_magic(int fd);
static void precompile(list_t *instructions, list_t **label_poses, uint32_t *prog_size);
static uint8_t get_instruction_size(t_instruction *instruction);
static bool has_argument_coding_byte(t_op op);
static uint8_t get_arg_size(t_op op, t_arg_type arg_type);
static bool op_has_idx(t_op op);
static void append_label_to_label_poses(list_t **label_poses, uint32_t cur_pos, char const *label);
static void write_header(int fd, t_header *header, uint32_t prog_size);
static void write_fixed_size(int fd, char const *str, int size);
static void expand_file(int fd, int expand_size);
static void write_instructions(int fd, list_t *instructions, list_t *label_poses);
static void write_instruction(int fd, t_instruction *instruction, list_t *label_poses, uint32_t *cur_pos);
static uint8_t get_argument_coding_byte(t_instruction *instruction);
static void write_arg(int fd, t_arg *arg, bool has_idx, list_t *label_poses, uint32_t cur_pos);
static uint32_t get_label_pos(list_t *label_poses, char const *label);
static void lst_wrapper_safe_free_label_pos(void *label_pos_void);


void compile_to_file(char const *compiled_file_name, t_header *header, list_t *instructions)
{
	list_t *label_poses = NULL;
	uint32_t prog_size = 0;
	int fd;

	precompile(instructions, &label_poses, &prog_size);

	fd = safe_open(compiled_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write_magic(fd);
	write_header(fd, header, prog_size);
	write_instructions(fd, instructions, label_poses);

	safe_lstclear(&label_poses, &lst_wrapper_safe_free_label_pos);
	safe_close(fd);
}

static void precompile(list_t *instructions, list_t **label_poses, uint32_t *prog_size)
{
	list_t *cur_instruction_node;
	t_instruction *cur_instruction;
	uint8_t instruction_size;

	*prog_size = 0;
	*label_poses = NULL;

	cur_instruction_node = instructions;
	while (cur_instruction_node)
	{
		cur_instruction = (t_instruction *)cur_instruction_node->content;

		instruction_size = get_instruction_size(cur_instruction);
		if (cur_instruction->label)
			append_label_to_label_poses(label_poses, *prog_size, cur_instruction->label);
		*prog_size += instruction_size;

		cur_instruction_node = cur_instruction_node->next;
	}
}

static uint8_t get_instruction_size(t_instruction *instruction)
{
	uint8_t instruction_size;

	instruction_size = 1; /* op code */
	instruction_size += (has_argument_coding_byte(instruction->op) ? 1 : 0);
	for (int i = 0; i < instruction->n_args; i++)
	{
		instruction_size += get_arg_size(instruction->op, instruction->args[i].arg_type);
	}

	return instruction_size;
}

static uint8_t get_arg_size(t_op op, t_arg_type arg_type)
{
	switch (arg_type)
	{
		case DIRECT_LABEL:
		case DIRECT_NUMERIC:
			return (op_has_idx(op) ? IND_ARG_SIZE : DIR_ARG_SIZE);
		case INDIRECT_LABEL:
		case INDIRECT_NUMERIC:
			return IND_ARG_SIZE;
		case REG:
			return REG_ARG_SIZE;
		default:
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);
			return 0; /* never reached */
	}
}

static bool op_has_idx(t_op op)
{
	switch (op)
	{
		case ALIVE:
		case LOAD:
		case STORE:
		case ADDITION:
		case SUBSTRACTION:
		case AND:
		case OR:
		case XOR:
		case LONG_LOAD:
		case AFF:
			return false;
		case ZJMP:
		case LOAD_INDEX:
		case STORE_INDEX:
		case FORK:
		case LONG_LOAD_INDEX:
		case LONG_FORK:
			return true;
		default:
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR); /* never reached */
			return false; /* never reached */
	}
}

static void append_label_to_label_poses(list_t **label_poses, uint32_t cur_pos, char const *label)
{
	list_t *node;
	t_label_pos *label_pos;

	label_pos = safe_malloc(sizeof(*label_pos));
	label_pos->label_ref = label;
	label_pos->pos = cur_pos;
	node = safeize_malloc(ft_lstnew(label_pos));
	ft_lstadd_back(label_poses, node);
}

static void write_magic(int fd)
{
	uint8_t corewar_magic[] = COREWAR_EXEC_MAGIC;

	safe_write(fd, &corewar_magic[0], sizeof(corewar_magic));
}

static void write_header(int fd, t_header *header, uint32_t prog_size)
{
	uint32_t prog_size_big_endian = ft_htonl(prog_size);

	write_fixed_size(fd, header->name, PROG_NAME_LENGTH);
	safe_write(fd, &prog_size_big_endian, sizeof(prog_size_big_endian));
	write_fixed_size(fd, header->comment, COMMENT_LENGTH);
}

static void write_fixed_size(int fd, char const *str, int size)
{
	expand_file(fd, size);
	safe_write(fd, str, ft_strlen(str));
	safe_lseek(fd, size - ft_strlen(str), SEEK_CUR);
}

static void expand_file(int fd, int expand_size)
{
	int zero = 0;

	safe_lseek(fd, expand_size - 1 /* written after */, SEEK_CUR);
	safe_write(fd, &zero, 1);
	safe_lseek(fd, -expand_size, SEEK_CUR);
}

static void write_instructions(int fd, list_t *instructions, list_t *label_poses)
{
	list_t *cur_instruction_node;
	t_instruction *cur_instruction;
	uint32_t cur_pos;

	cur_pos = 0;
	cur_instruction_node = instructions;
	while (cur_instruction_node)
	{
		cur_instruction = (t_instruction *)cur_instruction_node->content;
		write_instruction(fd, cur_instruction, label_poses, &cur_pos);
		cur_instruction_node = cur_instruction_node->next;
	}
}

static void write_instruction(int fd, t_instruction *instruction, list_t *label_poses, uint32_t *cur_pos)
{
	uint8_t argument_coding_byte;

	safe_write(fd, &instruction->op, 1);

	if (has_argument_coding_byte(instruction->op))
	{
		argument_coding_byte = get_argument_coding_byte(instruction);
		safe_write(fd, &argument_coding_byte, 1);
	}

	for (int i = 0; i < instruction->n_args; i++)
	{
		write_arg(fd, &instruction->args[i], op_has_idx(instruction->op), label_poses, *cur_pos);
	}
	*cur_pos += get_instruction_size(instruction);
}

static bool has_argument_coding_byte(t_op op)
{
	switch (op)
	{
		case ALIVE:
		case ZJMP:
		case FORK:
			return false;
		case LOAD:
		case STORE:
		case ADDITION:
		case SUBSTRACTION:
		case AND:
		case OR:
		case XOR:
		case LOAD_INDEX:
		case STORE_INDEX:
		case LONG_LOAD:
		case LONG_LOAD_INDEX:
		case LONG_FORK:
		case AFF:
			return true;
		default:
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);
			return false; /* never reached */
	}
}

static uint8_t get_argument_coding_byte(t_instruction *instruction)
{
	uint8_t argument_coding_byte;

	argument_coding_byte = 0;
	for (int i = 0; i < instruction->n_args; i++)
	{
		switch (instruction->args[i].arg_type)
		{
			case REG:
				argument_coding_byte |= REG_CODE;
				break;
			case DIRECT_LABEL:
			case DIRECT_NUMERIC:
				argument_coding_byte |= DIR_CODE;
				break;
			case INDIRECT_LABEL:
			case INDIRECT_NUMERIC:
				argument_coding_byte |= IND_CODE;
				break;
		}
		argument_coding_byte <<= 2;
	}
	argument_coding_byte <<= 2 * (3 - instruction->n_args);
	return argument_coding_byte;
}

static void write_arg(int fd, t_arg *arg, bool has_idx, list_t *label_poses, uint32_t cur_pos)
{
	uint32_t numeric_value_big_endian;
	uint32_t label_pos;

	if (arg->arg_type == DIRECT_LABEL || arg->arg_type == INDIRECT_LABEL)
	{
		label_pos = get_label_pos(label_poses, arg->label);
		if (label_pos == UINT_MAX)
			die(EXIT_STATUS_COMPILE_INSTR, EXIT_MESSAGE_COMPILE_INSTR_LABEL_MISSING);
		numeric_value_big_endian = ft_htonl(label_pos - cur_pos);
	}
	else if (arg->arg_type == DIRECT_NUMERIC || arg->arg_type == INDIRECT_NUMERIC)
	{
		numeric_value_big_endian = ft_htonl(arg->numeric_value);
	}

	if (arg->arg_type == REG)
		safe_write(fd, &arg->reg_number, 1);
	else if (((arg->arg_type == DIRECT_NUMERIC || arg->arg_type == DIRECT_LABEL) && has_idx)
		  || (arg->arg_type == INDIRECT_NUMERIC || arg->arg_type == INDIRECT_LABEL))
		safe_write(fd, ((uint8_t *)&numeric_value_big_endian) + 2, 2);
	else if (arg->arg_type == DIRECT_NUMERIC || arg->arg_type == DIRECT_LABEL)
		safe_write(fd, &numeric_value_big_endian, 4);
	else
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR); /* never reached */
}

static uint32_t get_label_pos(list_t *label_poses, char const *label)
{
	list_t *cur_label_pos_node;
	t_label_pos *cur_label_pos;

	cur_label_pos_node = label_poses;
	while (cur_label_pos_node)
	{
		cur_label_pos = (t_label_pos *)cur_label_pos_node->content;
		if (ft_strequal((char *)cur_label_pos->label_ref, (char *)label))
			return cur_label_pos->pos;
		cur_label_pos_node = cur_label_pos_node->next;
	}
	return UINT_MAX;
}

static void lst_wrapper_safe_free_label_pos(void *label_pos_void)
{
	t_label_pos *label_pos;

	label_pos = (t_label_pos *)label_pos_void;
	safe_free(label_pos);
}
