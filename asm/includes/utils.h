#ifndef UTILS_H
# define UTILS_H

# include <stdbool.h>

# define STR(X) STR2(X)
# define STR2(X) #X
# define GET_ARRAY_LEN(X) (sizeof(X) / sizeof(*X))

bool strend_with(char const *str, char const *suffix);
void assert(bool condition, int exit_status, char const *err_msg);
void die(int status, char const *err);
char const *utils_strchr_set(char const *s, char const *set);
uint32_t ft_htonl(uint32_t hostlong);
uint32_t ft_htons(uint16_t hostlong);

#endif
