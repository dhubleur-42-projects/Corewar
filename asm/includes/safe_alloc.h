#ifndef SAFE_ALLOC_H
# define SAFE_ALLOC_H

# include <stddef.h>

# include "libft.h"

void *safe_malloc(size_t size);
void safe_free(void *ptr);
void *safeize_malloc(void *ptr);
void safe_free_all(void);
void safe_lstclear(list_t **lst, void (*del)(void *));

#endif
