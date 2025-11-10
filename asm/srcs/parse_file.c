#include <fcntl.h>
#include <stdbool.h>

#include "corewar.h"
#include "exit_message.h"
#include "exit_status.h"
#include "libft.h"
#include "safe_alloc.h"
#include "safe_exit.h"
#include "safe_open.h"
#include "utils.h"

#include "parse_file.h"

typedef struct s_file_tracker
{
	int fd;
	char const *cur_line;
} t_file_tracker;

static bool parse_header(t_file_tracker *file_tracker, t_header *header_out);
static bool seek_to_next_line(t_file_tracker *file_tracker);
static void clean_line(char const *line, char *cleaned_line);
static void parse_header_line(char const *line, t_header *header_out);
static char const *get_value_ptr(char const *line);
static void assert_value_is_valid(char const *value_ptr);
static bool is_split_character(char c);

void parse_file(char const *champion_file_name)
{
	t_header header;
	t_file_tracker file_tracker;
	
	file_tracker.fd = safe_open(champion_file_name, O_RDONLY, 0);
	parse_header(&file_tracker, &header);
}

static bool parse_header(t_file_tracker *file_tracker, t_header *header_out)
{
	char cleaned_line[LINE_MAX_LEN + 1];

	header_out->name = NULL;
	header_out->comment = NULL;
	while (header_out->name == NULL || header_out->comment == NULL)
	{
		if (!seek_to_next_line(file_tracker))
			return false;
		clean_line(file_tracker->cur_line, cleaned_line);
		parse_header_line(cleaned_line, header_out);
	}
	return true;
}

static bool seek_to_next_line(t_file_tracker *file_tracker)
{
	file_tracker->cur_line = safeize_malloc(get_next_line(file_tracker->fd));
	return file_tracker->cur_line != NULL;
}

static void clean_line(char const *line, char *cleaned_line)
{
	char *tmp_line;

	tmp_line = safeize_malloc(ft_strtrim(line, " \t\n"));
	for (char *cur_ptr = tmp_line; *cur_ptr; cur_ptr++)
	{
		if (*cur_ptr == '#')
		{
			*cur_ptr = 0;
			break;
		}
	}
	ft_strcpy(cleaned_line, tmp_line);
	safe_free(tmp_line);
}

static void parse_header_line(char const *line, t_header *header_out)
{
	char const *value_ptr;

	if (*line == 0)
		return;
	value_ptr = get_value_ptr(line);
	assert_value_is_valid(value_ptr);

	if (ft_strncmp(line, ".name", sizeof(".name") - 1) == 0)
	{
		assert(header_out->name == NULL, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_NAME);
		assert(ft_strlen(value_ptr) < PROG_NAME_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_NAME_TOO_LONG);
		header_out->name = safeize_malloc(ft_strdup(value_ptr + 1 /* skipping first '"' */));
		header_out->name[ft_strlen(header_out->name) - 1] = 0;
	}
	else if (ft_strncmp(line, ".comment", sizeof(".comment") - 1) == 0)
	{
		assert(header_out->comment == NULL, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_COMMENT);
		assert(ft_strlen(value_ptr) < COMMENT_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, EXIT_MESSAGE_PARSING_HEADER_COMMENT_TOO_LONG);
		header_out->comment = safeize_malloc(ft_strdup(value_ptr + 1 /* skipping first '"' */));
		header_out->comment[ft_strlen(header_out->comment) - 1] = 0;
	}
	else
		die(EXIT_MESSAGE_PARSING_HEADER_UNKNOWN_INSTRUCTION, EXIT_STATUS_PARSING_HEADER);
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

static bool is_split_character(char c)
{
	return c == ' ' || c == '\t';
}
