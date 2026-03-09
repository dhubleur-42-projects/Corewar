#ifndef SAFE_GET_NEXT_LINE_H
# define SAFE_GET_NEXT_LINE_H

char *safe_get_next_line(int fd);
void safe_get_next_line_free_all(void);
void safe_get_next_line_free(int fd);

#endif
