#ifndef SAFE_FILE_H
# define SAFE_FILE_H

# include <stddef.h>

int safe_open(char const *path, int flags, mode_t mode);
void safe_close(int fd);
void safe_open_append(int fd);
void safe_close_all(void);
void safe_write(int fd, const void *buf, size_t count);
void safe_lseek(int fd, off_t offset, int whence);

#endif
