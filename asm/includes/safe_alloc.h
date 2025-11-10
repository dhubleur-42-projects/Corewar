#ifndef SAFE_ALLOC_H
# define SAFE_ALLOC_H

# include <stddef.h>

void *safe_malloc(size_t size);
void safe_free(void *ptr);
void *safeize_malloc(void *ptr);
void safe_free_all(void);

#endif
