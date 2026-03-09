#include <fcntl.h>
#include <stdbool.h>

#include "corewar.h"
#include "exit_message.h"
#include "exit_status.h"
#include "libft.h"
#include "safe_alloc.h"
#include "safe_exit.h"
#include "safe_get_next_line.h"
#include "safe_file.h"
#include "utils.h"

#include "parse_file.h"


typedef struct s_arg_types
{
	t_arg_type arg_types[6];
	size_t n_args_types;
} t_arg_types;

typedef struct s_file_tracker
{
	int fd;
	char cur_line[LINE_MAX_LEN + 1];
	char *cur_pos_ptr;
} t_file_tracker;

static void parse_header(t_file_tracker *file_tracker, t_header *header_out);
static void parse_header_line(char const *line, t_header *header_out);
static char const *get_value_ptr(char const *line);
static bool is_split_character(char c);
static void assert_value_is_valid(char const *value_ptr);
static void parse_instructions(t_file_tracker *file_tracker, list_t **instructions);
static void seek_to_next_token(t_file_tracker *file_tracker);
static t_token_type get_token_type(char const *cur_token);
static void seek_to_next_cleaned_line(t_file_tracker *file_tracker);
static void clean_line(char const *line, char *cleaned_line);
static char *clean_token(char const *token);
static bool is_eof(t_file_tracker *file_tracker);
static char *extract_label(char *token);
static char **split_op(char *token);
static char *loop_until_not_char(char *str, char *set);
static void parse_instruction(char **op_split, t_instruction *instruction);
static void parse_op(char *raw_op, t_op *op);
static t_arg_types *get_args_types_map_from_op(t_op op);
static t_arg_types *dup_arg_type_map(t_arg_types arg_types_map[4]);
static void parse_arg(char const *raw_arg, t_arg_types *arg_type_map, t_arg *arg);
static bool get_arg_type(char const *raw_arg, t_arg_type *arg_type);
static bool is_arg_type_in_map(t_arg_type arg_type, t_arg_types *arg_type_map);
static void parse_arg_direct_label(char const *raw_arg, t_arg *arg);
static void parse_arg_direct_numeric(char const *raw_arg, t_arg *arg);
static void parse_arg_indirect_label(char const *raw_arg, t_arg *arg);
static bool is_label(char const *label);
static void parse_arg_indirect_numeric(char const *raw_arg, t_arg *arg);
static void parse_arg_reg(char const *raw_arg, t_arg *arg);
static bool is_numeric(char const *numeric);
static void append_instruction(list_t **instructions, t_instruction *instruction);
static void safe_free_split(char **split_tab);


void parse_file(char const *champion_file_name, t_header *header, list_t **instructions)
{
	t_file_tracker file_tracker;

	file_tracker.fd = safe_open(champion_file_name, O_RDONLY, 0);

	parse_header(&file_tracker, header);
	parse_instructions(&file_tracker, instructions);

	safe_get_next_line_free(file_tracker.fd);
	safe_close(file_tracker.fd);
}

static void parse_header(t_file_tracker *file_tracker, t_header *header_out)
{
	header_out->name = NULL;
	header_out->comment = NULL;
	while (header_out->name == NULL || header_out->comment == NULL)
	{
		seek_to_next_cleaned_line(file_tracker);
		if (file_tracker->cur_line[0] == '\0' /* end of file */)
			die(EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_UNCOMPLETE_PARSE);
		parse_header_line(file_tracker->cur_line, header_out);
	}
}

static void parse_header_line(char const *line, t_header *header_out)
{
	char const *value_ptr;

	if (*line == 0)
		return;
	value_ptr = get_value_ptr(line);

	if (ft_strncmp(line, ".name", sizeof(".name") - 1) == 0)
	{
		assert_value_is_valid(value_ptr);
		assert(header_out->name == NULL, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_NAME);
		assert(ft_strlen(value_ptr) <= PROG_NAME_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_NAME_TOO_LONG);
		header_out->name = safeize_malloc(ft_strdup(value_ptr + 1 /* skipping first '"' */));
		header_out->name[ft_strlen(header_out->name) - 1] = 0;
	}
	else if (ft_strncmp(line, ".comment", sizeof(".comment") - 1) == 0)
	{
		assert_value_is_valid(value_ptr);
		assert(header_out->comment == NULL, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_COMMENT);
		assert(ft_strlen(value_ptr) <= COMMENT_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_COMMENT_TOO_LONG);
		header_out->comment = safeize_malloc(ft_strdup(value_ptr + 1 /* skipping first '"' */));
		header_out->comment[ft_strlen(header_out->comment) - 1] = 0;
	}
	else
		die(EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_UNKNOWN_INSTRUCTION);
}

static char const *get_value_ptr(char const *line)
{
	char const *value_ptr;

	value_ptr = line;
	for (;*value_ptr && !is_split_character(*value_ptr);value_ptr++)
		;
	for (;*value_ptr && is_split_character(*value_ptr);value_ptr++)
		;
	return value_ptr;
}

static bool is_split_character(char c)
{
	return c == ' ' || c == '\t';
}

static void assert_value_is_valid(char const *value_ptr)
{
	char const *tmp_ptr;

	assert(*value_ptr != 0, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_VALUE_MISSING);
	assert(*value_ptr == '"', EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_VALUE_NOT_QUOTED);
	for (tmp_ptr = value_ptr + 1; *tmp_ptr && *tmp_ptr != '"'; tmp_ptr++)
		;
	assert(*tmp_ptr == '"', EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_VALUE_NOT_QUOTED);
	assert(*(tmp_ptr + 1) == 0, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_VALUE_MUST_END_QUOTE);
}

static void parse_instructions(t_file_tracker *file_tracker, list_t **instructions)
{
	t_token_type token_type;
	char *previous_label;
	char **op_split;
	char *cleaned_token;
	t_instruction instruction;

	previous_label = NULL;
	seek_to_next_token(file_tracker);
	cleaned_token = clean_token(file_tracker->cur_pos_ptr);
	while (!is_eof(file_tracker))
	{
		token_type = get_token_type(cleaned_token);
		if (token_type == LABEL)
		{
			if (previous_label != NULL)
				die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_TWO_LABELS_IN_A_ROW);
			previous_label = extract_label(cleaned_token);
		}
		else /* token_type == INSTRUCTION */
		{
			op_split = split_op(cleaned_token);
			parse_instruction(op_split, &instruction);
			instruction.label = previous_label;	/* alloc ownership transfer */
			append_instruction(instructions, &instruction);
			previous_label = NULL;
			safe_free_split(op_split);
		}
		seek_to_next_token(file_tracker);
		safe_free(cleaned_token);
		cleaned_token = clean_token(file_tracker->cur_pos_ptr);
	}
	safe_free(cleaned_token);
	safe_free(previous_label);
}

static void seek_to_next_token(t_file_tracker *file_tracker)
{
	t_token_type cur_token_type;

	cur_token_type = get_token_type(file_tracker->cur_pos_ptr);
	if (cur_token_type == LABEL)
	{
		file_tracker->cur_pos_ptr = ft_strchr(file_tracker->cur_pos_ptr, LABEL_CHAR);
		if (file_tracker->cur_pos_ptr != NULL)
			file_tracker->cur_pos_ptr++;
		else
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);
	}
	if (cur_token_type == INSTRUCTION || *file_tracker->cur_pos_ptr == '\0')
		seek_to_next_cleaned_line(file_tracker);
}

static t_token_type get_token_type(char const *cur_token)
{
	int i;

	for (i = 0; cur_token[i] && cur_token[i] != LABEL_CHAR && !ft_strchr(" \t", cur_token[i]); i++)
		;
	if (cur_token[i] == LABEL_CHAR)
		return LABEL;
	return INSTRUCTION;
}

static void seek_to_next_cleaned_line(t_file_tracker *file_tracker)
{
	char *new_line = NULL;

	do
	{
		safe_free(new_line);
		new_line = safe_get_next_line(file_tracker->fd);
		clean_line(new_line, file_tracker->cur_line);
	} while (new_line[0] != '\0' && file_tracker->cur_line[0] == '\0');
	file_tracker->cur_pos_ptr = file_tracker->cur_line;
	safe_free(new_line);
}

static void clean_line(char const *line, char *cleaned_line)
{
	char *tmp_line;
	char *tmp_line_2;

	tmp_line = safeize_malloc(ft_strdup(line));
	for (char *cur_ptr = tmp_line; *cur_ptr; cur_ptr++)
	{
		if (*cur_ptr == '#')
		{
			*cur_ptr = 0;
			break;
		}
	}
	tmp_line_2 = safeize_malloc(ft_strtrim(tmp_line, " \t\n"));
	ft_strcpy(cleaned_line, tmp_line_2);
	safe_free(tmp_line);
	safe_free(tmp_line_2);
}

static char *clean_token(char const *token)
{
	char *cleaned_token;
	t_token_type token_type;
	char *label_char_ptr;
	char *tmp_token;

	tmp_token = safeize_malloc(ft_strdup(token));
	token_type = get_token_type(tmp_token);
	if (token_type == LABEL)
	{
		label_char_ptr = ft_strchr(tmp_token, LABEL_CHAR);
		*(label_char_ptr + 1) = '\0';
	}
	cleaned_token = safeize_malloc(ft_strtrim(tmp_token, " \t\n"));
	safe_free(tmp_token);

	return cleaned_token;
}

static bool is_eof(t_file_tracker *file_tracker)
{
	return *file_tracker->cur_pos_ptr == '\0';
}

static char *extract_label(char *token)
{
	char *label_char_ptr;
	char *label;
	size_t label_size;

	label_char_ptr = ft_strchr(token, LABEL_CHAR);
	assert(label_char_ptr != NULL, EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);
	label_size = label_char_ptr - token + 1;
	label = safe_malloc(sizeof(char) * (label_size));
	ft_strlcpy(label, token, label_size); /* can't fail */
	return label;
}

static char **split_op(char *token)
{
	char **split;
	char *end_op_ptr;
	char *begin_params_ptr;
	char **args_split;
	int i;
	int n_params;

	end_op_ptr = (char *)utils_strchr_set(token, " \t");
	if (end_op_ptr == NULL) /* no params */
	{
		split = safe_malloc(sizeof(char **) * 2);
		split[0] = safeize_malloc(ft_strdup(token));
		split[1] = NULL;
		return split;
	}
	end_op_ptr--; /* real end, not after */

	begin_params_ptr = loop_until_not_char(end_op_ptr + 1, " \t");
	args_split = safeize_malloc(ft_split(begin_params_ptr, ','));
	for (n_params = 0; args_split[n_params]; n_params++)
		safeize_malloc(args_split[n_params]);
	split = safe_malloc(sizeof(char *) * (1 + n_params + 1) /* op + n params + NULL */);

	split[0] = safe_malloc(sizeof(char) * (end_op_ptr - token + 2 /* end + NULL */));
	ft_strlcpy(split[0], token, end_op_ptr - token + 2 /* end + NULL */);
	for (i = 0; args_split[i]; i++)
		split[i + 1] = safeize_malloc(ft_strtrim(args_split[i], " \t\n"));
	split[i + 1] = NULL;

	safe_free_split(args_split);
	return split;
}

static char *loop_until_not_char(char *str, char *set)
{
	char *ret;

	for (ret = str; ft_strchr(set, *ret); ret++)
		;

	return ret;
}

static void parse_instruction(char **op_split, t_instruction *instruction)
{
	char **raw_args;
	t_arg_types *args_types_map;

	if (!op_split[0])
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);

	parse_op(op_split[0], &instruction->op);
	args_types_map = get_args_types_map_from_op(instruction->op);	/* can't fail */

	raw_args = op_split + 1;
	instruction->n_args = 0;
	for (int i = 0; raw_args[i]; i++)
	{
		parse_arg(raw_args[i], &args_types_map[i], &instruction->args[i]);
		instruction->n_args++;
	}
	safe_free(args_types_map);
}

static void parse_op(char *raw_op, t_op *op)
{
	struct {char *op_name; t_op op;} op_lut[] = 
	{
		{"live", ALIVE},
		{"ld", LOAD},
		{"st", STORE},
		{"add", ADDITION},
		{"sub", SUBSTRACTION},
		{"and", AND},
		{"or", OR},
		{"xor", XOR},
		{"zjmp", ZJMP},
		{"ldi", LOAD_INDEX},
		{"sti", STORE_INDEX},
		{"fork", FORK},
		{"lld", LONG_LOAD},
		{"lldi", LONG_LOAD_INDEX},
		{"lfork", LONG_FORK},
		{"aff", AFF},
	};
	for (unsigned long i = 0; i < GET_ARRAY_LEN(op_lut); i++)
	{
		if (ft_strequal(raw_op, op_lut[i].op_name))
		{
			*op = op_lut[i].op;
			return;
		}
	}

	/* op not found */
	die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_MALFORMED_INSTRUCTION);
}

static t_arg_types *get_args_types_map_from_op(t_op op)
{
	static struct {t_op op; t_arg_types args_types_map[4];} args_types_map_lut[] =
	{
		{ALIVE, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 2}}},
		{LOAD, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 4}, {.arg_types = {REG}, .n_args_types = 1}}},
		{STORE, {{.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 3}}},
		{ADDITION, {{.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG}, .n_args_types = 1}}},
		{SUBSTRACTION, {{.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG}, .n_args_types = 1}}},
		{AND, {{.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG}, .n_args_types = 1}}},
		{OR, {{.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG}, .n_args_types = 1}}},
		{XOR, {{.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG}, .n_args_types = 1}}},
		{ZJMP, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 2}}},
		{LOAD_INDEX, {{.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 3}, {.arg_types = {REG}, .n_args_types = 1}}},
		{STORE_INDEX, {{.arg_types = {REG}, .n_args_types = 1}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 3}}},
		{FORK, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 2}}},
		{LONG_LOAD, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 4}, {.arg_types = {REG}, .n_args_types = 1}}},
		{LONG_LOAD_INDEX, {{.arg_types = {REG, DIRECT_LABEL, DIRECT_NUMERIC, INDIRECT_LABEL, INDIRECT_NUMERIC}, .n_args_types = 5}, {.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC, REG}, .n_args_types = 3}, {.arg_types = {REG}, .n_args_types = 1}}},
		{LONG_FORK, {{.arg_types = {DIRECT_LABEL, DIRECT_NUMERIC}, .n_args_types = 2}}},
		{AFF, {{.arg_types = {REG}, .n_args_types = 1}}}
	};
	for (unsigned long i = 0; i < GET_ARRAY_LEN(args_types_map_lut); i++)
	{
		if (args_types_map_lut[i].op == op)
		{
			return dup_arg_type_map(args_types_map_lut[i].args_types_map);
		}
	}
	return NULL;
}

static t_arg_types *dup_arg_type_map(t_arg_types arg_types_map[4])
{
	t_arg_types *map;

	map = safe_malloc(sizeof(t_arg_types) * 4);
	for (int i = 0; i < 4; i++)
	{
		ft_memcpy(&map[i], &arg_types_map[i], sizeof(arg_types_map[i])); /* can't fail */
	}
	return map;
}

static void parse_arg(char const *raw_arg, t_arg_types *arg_type_map, t_arg *arg)
{
	t_arg_type arg_type;
	assert(get_arg_type(raw_arg, &arg_type), EXIT_STATUS_PARSING_INSTR,
		   EXIT_MESSAGE_PARSING_INSTR_MALFORMED_INSTRUCTION);
	assert(is_arg_type_in_map(arg_type, arg_type_map), EXIT_STATUS_PARSING_INSTR,
		   EXIT_MESSAGE_PARSING_INSTR_MALFORMED_INSTRUCTION);
	switch(arg_type)
	{
		case DIRECT_LABEL:
			parse_arg_direct_label(raw_arg, arg);
			break;
		case DIRECT_NUMERIC:
			parse_arg_direct_numeric(raw_arg, arg);
			break;
		case INDIRECT_LABEL:
			parse_arg_indirect_label(raw_arg, arg);
			break;
		case INDIRECT_NUMERIC:
			parse_arg_indirect_numeric(raw_arg, arg);
			break;
		case REG:
			parse_arg_reg(raw_arg, arg);
			break;
		default:
			die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR); /* never reached */
			break;
	}
}

static bool get_arg_type(char const *raw_arg, t_arg_type *arg_type)
{
	if (raw_arg[0] == 'r')
		*arg_type = REG;
	else if (raw_arg[0] == LABEL_CHAR)
		*arg_type = INDIRECT_LABEL;
	else if (ft_isdigit(raw_arg[0]) || raw_arg[0] == '-')
		*arg_type = INDIRECT_NUMERIC;
	else if (raw_arg[0] == '%')
	{
		if (raw_arg[1] == LABEL_CHAR)
			*arg_type = DIRECT_LABEL;
		else if (ft_isdigit(raw_arg[1]) || raw_arg[1] == '-')
			*arg_type = DIRECT_NUMERIC;
		else
			return false;
	}
	else
		return false;

	return true;
}

static bool is_arg_type_in_map(t_arg_type arg_type, t_arg_types *arg_type_map)
{
	for (size_t i = 0; i < arg_type_map->n_args_types; i++)
	{
		if (arg_type == arg_type_map->arg_types[i])
			return true;
	}
	return false;
}

static void parse_arg_direct_label(char const *raw_arg, t_arg *arg)
{
	char const *label_ref;

	if (raw_arg[0] != '%')
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);
	if (raw_arg[1] != LABEL_CHAR)
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);

	label_ref = raw_arg + 2;
	if (!is_label(label_ref))
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_BAD_LABEL_FORMAT);
	arg->arg_type = DIRECT_LABEL;
	arg->label = safeize_malloc(ft_strdup(label_ref));
}

static void parse_arg_direct_numeric(char const *raw_arg, t_arg *arg)
{
	char const *raw_num_ref;

	if (raw_arg[0] != '%')
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);

	raw_num_ref = raw_arg + 1;
	if (!is_numeric(raw_num_ref))
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_BAD_NUMERIC_FORMAT);
	arg->arg_type = DIRECT_NUMERIC;
	arg->numeric_value = ft_atoi(raw_num_ref);
}

static void parse_arg_indirect_label(char const *raw_arg, t_arg *arg)
{
	char const *label_ref;

	if (raw_arg[0] != LABEL_CHAR)
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);

	label_ref = raw_arg + 1;
	if (!is_label(label_ref))
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_BAD_LABEL_FORMAT);
	arg->arg_type = INDIRECT_LABEL;
	arg->label = safeize_malloc(ft_strdup(label_ref));
}

static bool is_label(char const *label)
{
	if (!label[0])
		return false;
	for (int i = 0; label[i]; i++)
	{
		if (!ft_strchr(LABEL_CHARS, label[i]))
			return false;
	}
	return true;
}

static void parse_arg_indirect_numeric(char const *raw_arg, t_arg *arg)
{
	if (!is_numeric(raw_arg))
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_BAD_NUMERIC_FORMAT);
	arg->arg_type = INDIRECT_NUMERIC;
	arg->numeric_value = ft_atoi(raw_arg);
}

static void parse_arg_reg(char const *raw_arg, t_arg *arg)
{
	char const *raw_num_ref;
	int reg_nbr;

	if (raw_arg[0] != 'r')
		die(EXIT_STATUS_UNEXPECTED_ERROR, EXIT_MESSAGE_UNEXPECTED_ERROR);

	raw_num_ref = raw_arg + 1;
	if (!is_numeric(raw_num_ref))
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_INVALID_REGISTER);
	reg_nbr = ft_atoi(raw_num_ref);
	if (reg_nbr > REG_NUMBER)
		die(EXIT_STATUS_PARSING_INSTR, EXIT_MESSAGE_PARSING_INSTR_INVALID_REGISTER);

	arg->arg_type = REG;
	arg->reg_number= reg_nbr;
}

static bool is_numeric(char const *numeric)
{
	int i;

	if (!numeric[0])
		return false;

	i = 0;
	if (numeric[i] == '-')
		i++;
	for (; numeric[i]; i++)
	{
		if (!ft_isdigit(numeric[i]))
			return false;
	}
	return true;
}

static void append_instruction(list_t **instructions, t_instruction *instruction)
{
	list_t *node;
	t_instruction *duplicated_instruction;

	duplicated_instruction = safe_malloc(sizeof(*instruction));
	ft_memcpy(duplicated_instruction, instruction, sizeof(*instruction));	/* can't fail */
	node = safeize_malloc(ft_lstnew(duplicated_instruction));
	ft_lstadd_back(instructions, node);
}

static void safe_free_split(char **split_tab)
{
	for (char **cur = split_tab; *cur; cur++)
		safe_free(*cur);
	safe_free(split_tab);
}
