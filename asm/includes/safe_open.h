#ifndef SAFE_OPEN_H
# define SAFE_OPEN_H

# include <stddef.h>

int safe_open(char const *path, int flags, mode_t mode);
void safe_close(int fd);
void safe_open_append(int fd);
void safe_close_all(void);

#endif
