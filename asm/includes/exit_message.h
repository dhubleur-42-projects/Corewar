#ifndef EXIT_MESSAGE_H
# define EXIT_MESSAGE_H

# include "corewar.h"
# include "utils.h"

/* Generic */
# define EXIT_MESSAGE_MEMORY_ALLOCATION "Memory allocation error occured"
# define EXIT_MESSAGE_OPEN_FILE "An error occured while opening file"

/* Parse args */
# define EXIT_MESSAGE_PROG_NAME_TOO_LONG "Program name must not exceed "STR(FILENAME_MAX_LEN)" characters\n"

/* Parse header */
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_MISSING "A header value is missing"
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_NOT_QUOTED "Header values must be surrounded by '\"'"
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_MUST_END_QUOTE "Header values must end by '\"'"
# define EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_NAME "Duplicate .name instruction"
# define EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_COMMENT "Duplicate .comment instruction"
# define EXIT_MESSAGE_PARSING_HEADER_NAME_TOO_LONG ".name value too long"
# define EXIT_MESSAGE_PARSING_HEADER_COMMENT_TOO_LONG ".comment value too long"
# define EXIT_MESSAGE_PARSING_HEADER_UNKNOWN_INSTRUCTION "An error occured while parsing header: unknown instruction"

#endif
