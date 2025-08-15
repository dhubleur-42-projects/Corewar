#include "arguments.h"

bool is_champion_file(char *filename) {
	if (strlen(filename) < 4) {
		return false;
	}
	return strcmp(filename + strlen(filename) - 4, ".cor") == 0;
}

int count_champions(int argc, char **argv) {
	int count = 0;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-' && is_champion_file(argv[i])) {
			count++;
		}
	}
	return count;
}

bool is_positive_integer(char *str) {
	for (size_t i = 0; i < strlen(str); i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}
	return strlen(str) > 0;
}

void free_arguments(arguments_t *args) {
	if (args->champions != NULL) {
		for (int i = 0; i < args->number_of_champions; i++) {
			free(args->champions[i].filename);
		}
		free(args->champions);
		args->champions = NULL;
	}
}

bool parse_arguments(int argc, char **argv, arguments_t *args) {
	args->dump_cycle = -1;
	args->champions = NULL;
	args->number_of_champions = count_champions(argc, argv);
	
	if (args->number_of_champions == 0) {
		fprintf(stderr, "Error: No champions provided.\n");
		return false;
	}

	args->champions = malloc(args->number_of_champions * sizeof(champion_t));
	if (!args->champions) {
		fprintf(stderr, "Error: Memory allocation failed.\n");
		return false;
	}

	for (int i = 0; i < args->number_of_champions; i++) {
		args->champions[i].number = i + 1;
		args->champions[i].filename = NULL;
	}

	parsing_state_t state = CHAMPION;
	int last_champion_number = -1;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (state != CHAMPION) {
				// If we are not int CHAMPION state, the last argument was an option and need a value
				fprintf(stderr, "Error: Option %s needs an argument.\n", argv[i - 1]);
				return false;
			}

			if (strcmp(argv[i], "-dump") == 0) {
				state = DUMP;
			} else if (strcmp(argv[i], "-n") == 0) {
				state = NUMBER;
			} else {
				fprintf(stderr, "Error: Unknown option %s.\n", argv[i]);
				return false;
			}
		} else {
			if (state != CHAMPION) {
				// If we are not in CHAMPION state, the last argument was an option
				if (!is_positive_integer(argv[i])) {
					fprintf(stderr, "Error: Argument for %s must be a positive integer.\n", argv[i - 1]);
					return false;
				}
				if (state == DUMP) {
					args->dump_cycle = atoi(argv[i]);
					state = CHAMPION;
				} else if (state == NUMBER) {
					int champion_number = atoi(argv[i]);
					if (champion_number <= 0 || champion_number > args->number_of_champions) {
						fprintf(stderr, "Error: Champion number %d is out of range.\n", champion_number);
						return false;
					}
					last_champion_number = champion_number;
					state = CHAMPION;
				}
			} else {
				// We are in CHAMPION state, so this should be champion file
				char *filename = argv[i];
				if (!is_champion_file(filename)) {
					fprintf(stderr, "Error: Champion file %s must have a .cor extension.\n", filename);
					return false;
				}
				if (last_champion_number != -1) {
					if (args->champions[last_champion_number - 1].filename != NULL) {
						fprintf(stderr, "Error: Champion number %d already has a file assigned.\n", last_champion_number);
						return false;
					}
					args->champions[last_champion_number - 1].filename = strdup(filename);
				} else {
					for (int j = 0; j < args->number_of_champions; j++) {
						if (args->champions[j].filename == NULL) {
							args->champions[j].filename = strdup(filename);
							break;
						}
					}
				}
				last_champion_number = -1;
			}
		}
	}
	return true;
}