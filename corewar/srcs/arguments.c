#include "arguments.h"

bool is_champion_file(char *filename) {
	if (ft_strlen(filename) < 4) {
		return false;
	}
	return ft_strcmp(filename + ft_strlen(filename) - 4, ".cor") == 0;
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
	for (size_t i = 0; i < ft_strlen(str); i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}
	return ft_strlen(str) > 0;
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
		ft_dprintf(2,"Error: No champions provided.\n");
		return false;
	}
	if (args->number_of_champions > CHAMP_MAX_COUNT) {
		ft_dprintf(2,"Error: Too many champions provided. Maximum is %d.\n", CHAMP_MAX_COUNT);
		return false;
	}

	args->champions = malloc(args->number_of_champions * sizeof(champion_t));
	if (!args->champions) {
		ft_dprintf(2,"Error: Memory allocation failed.\n");
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
				ft_dprintf(2,"Error: Option %s needs an argument.\n", argv[i - 1]);
				return false;
			}

			if (ft_strcmp(argv[i], "-dump") == 0) {
				state = DUMP;
			} else if (ft_strcmp(argv[i], "-n") == 0) {
				state = NUMBER;
			} else {
				ft_dprintf(2,"Error: Unknown option %s.\n", argv[i]);
				return false;
			}
		} else {
			if (state != CHAMPION) {
				// If we are not in CHAMPION state, the last argument was an option
				if (!is_positive_integer(argv[i])) {
					ft_dprintf(2,"Error: Argument for %s must be a positive integer.\n", argv[i - 1]);
					return false;
				}
				if (state == DUMP) {
					args->dump_cycle = ft_atoi(argv[i]);
					state = CHAMPION;
				} else if (state == NUMBER) {
					int champion_number = ft_atoi(argv[i]);
					if (champion_number <= 0 || champion_number > args->number_of_champions) {
						ft_dprintf(2,"Error: Champion number %d is out of range.\n", champion_number);
						return false;
					}
					last_champion_number = champion_number;
					state = CHAMPION;
				}
			} else {
				// We are in CHAMPION state, so this should be champion file
				char *filename = argv[i];
				if (!is_champion_file(filename)) {
					ft_dprintf(2,"Error: Champion file %s must have a .cor extension.\n", filename);
					return false;
				}
				if (last_champion_number != -1) {
					if (args->champions[last_champion_number - 1].filename != NULL) {
						ft_dprintf(2,"Error: Champion number %d already has a file assigned.\n", last_champion_number);
						return false;
					}
					args->champions[last_champion_number - 1].filename = ft_strdup(filename);
				} else {
					for (int j = 0; j < args->number_of_champions; j++) {
						if (args->champions[j].filename == NULL) {
							args->champions[j].filename = ft_strdup(filename);
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