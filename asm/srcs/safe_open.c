#include <fcntl.h>
#include <stdio.h>

#include "libft.h"

#include "exit_message.h"
#include "exit_status.h"
#include "safe_exit.h"

#include "safe_open.h"

static void destroy_node_from_value(list_t **lst, int fd, void (*del)(void *));
static void close_wrapper(void *content);

list_t *open_lst = NULL;

int safe_open(char const *path, int flags, mode_t mode)
{
	int fd;

	fd = open(path, flags, mode);
	if (fd == -1)
	{
		perror(EXIT_MESSAGE_OPEN_FILE);
		safe_exit(EXIT_STATUS_IO);
	}
	safe_open_append(fd);
	return fd;
}

void safe_close(int fd)
{
	destroy_node_from_value(&open_lst, fd, close_wrapper);
}

static void destroy_node_from_value(list_t **lst, int fd, void (*del)(void *))
{
	list_t **prev_node_next_ptr;
	list_t *cur_node;

	prev_node_next_ptr = lst;
	cur_node = *lst;
	for (; cur_node != NULL; prev_node_next_ptr = &cur_node->next, cur_node = cur_node->next)
	{
		if (cur_node->content == (void *)(long)fd)
		{
			del(cur_node->content);
			*prev_node_next_ptr = cur_node->next;
			free(cur_node);
			return;
		}
	}
}

void safe_open_append(int fd)
{
	list_t *node;

	node = ft_lstnew((void *)(long)fd);
	if (node == NULL)
	{
		close(fd);
		die(EXIT_MESSAGE_MEMORY_ALLOCATION, EXIT_STATUS_MEMORY);
	}
	ft_lstadd_back(&open_lst, node);
}

void safe_close_all(void)
{
	ft_lstclear(&open_lst, close_wrapper);
}

static void close_wrapper(void *content)
{
	close((int)(long)content);
}

