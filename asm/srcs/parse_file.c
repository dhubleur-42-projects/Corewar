#include <stdio.h>
#include <stdbool.h>

#include "libft.h"
#include "utils.h"

//TODO Put \n at each message

//TODO Put elsewhere
typedef struct s_file_tracker
{
	int fd;
	char const *cur_line;
} t_file_tracker;

//TODO Put elsewhere
typedef struct s_header
{
	char *name;
	char *comment; //TODO Change to comment
} t_header;

#define LINE_MAX_LEN 4096 //TODO Put elsewhere
#define EXIT_STATUS_MEMORY 3 //TODO Put elsewhere
#define EXIT_STATUS_PARSING_HEADER 4 //TODO Put elsewhere
#define PROG_NAME_LENGTH 128 //TODO Put elsewhere (Got from op.h)
#define COMMENT_LENGTH 2048 //TODO Put elsewhere (Got from op.h)

static bool parse_header(t_file_tracker *file_tracker, t_header *header_out);
static bool seek_to_next_line(t_file_tracker *file_tracker);
static void clean_line(char const *line, char *cleaned_line);
static void parse_header_line(char const *line, t_header *header_out);
static char const *get_value_ptr(char const *line);
static void assert_value_is_valid(char const *value_ptr);
static bool is_split_character(char c);
static void assert(bool condition, int exit_status, char const *err_msg);

//TODO Return structure with datas
void parse_file(char const *champion_file_name)
{
	t_header header;
	t_file_tracker file_tracker;
	
	file_tracker.fd = try_open(champion_file_name);
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

//TODO Elsewhere?
static bool seek_to_next_line(t_file_tracker *file_tracker)
{
	file_tracker->cur_line = get_next_line(file_tracker->fd);;
	return file_tracker->cur_line != NULL;
}

static void clean_line(char const *line, char *cleaned_line)
{
	char *tmp_line;

	tmp_line = ft_strtrim(line, " \t\n");
	assert(tmp_line != NULL, EXIT_STATUS_MEMORY, "Memory allocation error occured");
	for (char *cur_ptr = tmp_line; *cur_ptr; cur_ptr++)
	{
		if (*cur_ptr == '#')
		{
			*cur_ptr = 0;
			break;
		}
	}
	ft_strcpy(cleaned_line, tmp_line);
	free(tmp_line);
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
		assert(header_out->name == NULL, EXIT_STATUS_PARSING_HEADER, "Duplicate .name instruction");
		assert(ft_strlen(value_ptr) < PROG_NAME_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, ".name value too long");
		header_out->name = ft_strdup(value_ptr + 1 /* skipping first '"' */);
		assert(header_out->name != NULL, EXIT_STATUS_MEMORY, "Memory allocation error occured");
		header_out->name[ft_strlen(header_out->name) - 1] = 0;
	}
	else if (ft_strncmp(line, ".comment", sizeof(".comment") - 1) == 0)
	{
		assert(header_out->comment == NULL, EXIT_STATUS_PARSING_HEADER, "Duplicate .comment instruction");
		assert(ft_strlen(value_ptr) < COMMENT_LENGTH + 2 /* surronding '"' */, EXIT_STATUS_PARSING_HEADER, ".comment value too long");
		header_out->comment = ft_strdup(value_ptr + 1 /* skipping first '"' */);
		assert(header_out->comment != NULL, EXIT_STATUS_MEMORY, "Memory allocation error occured");
		header_out->comment[ft_strlen(header_out->comment) - 1] = 0;
	}
	else
	{
		ft_putstr_fd("An error occured while parsing header: unknown instruction", 1);
		exit(EXIT_STATUS_PARSING_HEADER);
	}
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

	assert(*value_ptr != 0, EXIT_STATUS_PARSING_HEADER, "A header value is missing");
	assert(*value_ptr == '"', EXIT_STATUS_PARSING_HEADER, "Header values must be surrounded by '\"'");
	for (tmp_ptr = value_ptr + 1; *tmp_ptr && *tmp_ptr != '"'; tmp_ptr++)
		;
	assert(*tmp_ptr == '"', EXIT_STATUS_PARSING_HEADER, "Header values must be surrounded by '\"'");
	assert(*(tmp_ptr + 1) == 0, EXIT_STATUS_PARSING_HEADER, "Header values must end by '\"'");
}

static bool is_split_character(char c)
{
	return c == ' ' || c == '\t';
}

static void assert(bool condition, int exit_status, char const *err_msg) //TODO Move this function elsewhere
{
	if (!condition)
	{
		//TODO Use die instead of putstr + exit?
		write(1, err_msg, ft_strlen(err_msg));
		exit(exit_status);
	}
}
