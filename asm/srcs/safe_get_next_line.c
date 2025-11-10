#include "libft.h"

#include "exit_message.h"
#include "exit_status.h"
#include "safe_alloc.h"
#include "utils.h"

#include "safe_get_next_line.h"

static void safe_get_next_line_free(int fd);

int open_file[1024];

char *safe_get_next_line(int fd)
{
	char *gnl_ret;

	if (fd >= 1024)
		return NULL;
	gnl_ret = get_next_line(fd);
	if (gnl_ret == NULL)
	{
		open_file[fd] = 0;
		die(EXIT_MESSAGE_READ_FILE, EXIT_STATUS_IO);
	}
	safeize_malloc((void *)gnl_ret);
	open_file[fd] = 1;
	return gnl_ret;
}

void safe_get_next_line_free_all(void)
{
	for (int fd = 0; fd < 1024; fd++)
	{
		if (open_file[fd])
			safe_get_next_line_free(fd);
	}
}

static void safe_get_next_line_free(int fd)
{
	char *line;

	for (line = safe_get_next_line(fd); line[0] != '\0'; line = safe_get_next_line(fd))
		safe_free(line);
}
