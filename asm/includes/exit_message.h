#ifndef EXIT_MESSAGE_H
# define EXIT_MESSAGE_H

# include "corewar.h"
# include "utils.h"

/* Generic */
# define EXIT_MESSAGE_MEMORY_ALLOCATION "Memory allocation error occured\n"
# define EXIT_MESSAGE_OPEN_FILE "An error occured while opening file\n"
# define EXIT_MESSAGE_READ_FILE "An error occured while reading file\n"
# define EXIT_MESSAGE_WRITE_TO_FILE "An error occured while writing to file\n"
# define EXIT_MESSAGE_LSEEK "An error occured while seeking in file\n"
# define EXIT_MESSAGE_UNEXPECTED_ERROR "An unexpected error occured\n"

/* Parse args */
# define EXIT_MESSAGE_PROG_NAME_TOO_LONG "Program name must not exceed "STR(FILENAME_MAX_LEN)" characters\n"

/* Parse header */
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_MISSING "A header value is missing\n"
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_NOT_QUOTED "Header values must be surrounded by '\"'\n"
# define EXIT_MESSAGE_PARSING_HEADER_VALUE_MUST_END_QUOTE "Header values must end by '\"'\n"
# define EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_NAME "Duplicate .name instruction\n"
# define EXIT_MESSAGE_PARSING_HEADER_DUPLICATE_COMMENT "Duplicate .comment instruction\n"
# define EXIT_MESSAGE_PARSING_HEADER_NAME_TOO_LONG ".name value too long\n"
# define EXIT_MESSAGE_PARSING_HEADER_COMMENT_TOO_LONG ".comment value too long\n"
# define EXIT_MESSAGE_PARSING_HEADER_UNKNOWN_INSTRUCTION "An error occured while parsing header: unknown instruction\n"
# define EXIT_MESSAGE_PARSING_HEADER_UNCOMPLETE_PARSE ".name and/or .comment not found\n"

/* Parse instructions */
# define EXIT_MESSAGE_PARSING_INSTR_MALFORMED_INSTRUCTION "Instruction is malformed\n"
# define EXIT_MESSAGE_PARSING_INSTR_BAD_LABEL_FORMAT "Label format is invalid\n"
# define EXIT_MESSAGE_PARSING_INSTR_BAD_NUMERIC_FORMAT "Numeric format is invalid\n"
# define EXIT_MESSAGE_PARSING_INSTR_INVALID_REGISTER "Register is invalid\n"
# define EXIT_MESSAGE_PARSING_INSTR_TWO_LABELS_IN_A_ROW "Can't have two labels in a row\n"

/* Compile instructions */
#define EXIT_MESSAGE_COMPILE_INSTR_LABEL_MISSING "The label is missing\n"

#endif
