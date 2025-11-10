#ifndef UTILS_H
# define UTILS_H

# include <stdbool.h>

# define STR(X) STR2(X)
# define STR2(X) #X

bool strend_with(char const *str, char const *suffix);
int try_open(char const *filename);
void assert(bool condition, int exit_status, char const *err_msg);
void die(char const *err, int status);

#endif
