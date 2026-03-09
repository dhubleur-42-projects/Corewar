#include "libft.h"

#include "enable_debug_message.h"
#include "exit_message.h"
#include "exit_status.h"
#include "safe_alloc.h"

static void destroy_node_from_value(list_t **lst, void *ptr, void (*del)(void *));

list_t *allocated_lst = NULL;

void *safe_malloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	safeize_malloc(ptr);
	return ptr;
}

void safe_free(void *ptr)
{
	destroy_node_from_value(&allocated_lst, ptr, free);
}

static void destroy_node_from_value(list_t **lst, void *ptr, void (*del)(void *))
{
	list_t **prev_node_next_ptr;
	list_t *cur_node;

	prev_node_next_ptr = lst;
	cur_node = *lst;
	for (; cur_node != NULL; prev_node_next_ptr = &cur_node->next, cur_node = cur_node->next)
	{
		if (cur_node->content == ptr)
		{
			del(cur_node->content);
			*prev_node_next_ptr = cur_node->next;
			free(cur_node);
			return;
		}
	}
}

void *safeize_malloc(void *ptr)
{
	list_t *node;

	assert(ptr != NULL, EXIT_STATUS_MEMORY, EXIT_MESSAGE_MEMORY_ALLOCATION);
	node = ft_lstnew(ptr);
	if (node == NULL)
	{
		free(ptr);
		die(EXIT_STATUS_MEMORY, EXIT_MESSAGE_MEMORY_ALLOCATION);
	}
	ft_lstadd_back(&allocated_lst, node);
	return ptr;
}

void safe_free_all(void)
{
#ifdef DEBUG_SAFE_FREE_ALL
	if (allocated_lst)
		ft_dprintf(2, "Warning: some memory have not been free at free_all\n");
#else
	ft_lstclear(&allocated_lst, free);
#endif
}

void safe_lstclear(list_t **lst, void (*del)(void *))
{
	list_t	*ptr;
	list_t	*next;

	ptr = *lst;
	*lst = NULL;
	while (ptr != NULL)
	{
		next = ptr->next;
		(*del)(ptr->content);
		safe_free(ptr);
		ptr = next;
	}
}
