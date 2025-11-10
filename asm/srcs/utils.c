#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "libft.h"

#include "exit_status.h"
#include "exit_message.h"
#include "safe_exit.h"

#include "utils.h"

bool strend_with(char const *str, char const *suffix)
{
	unsigned int len = ft_strlen(str);
	unsigned int suffix_len = ft_strlen(suffix);
	unsigned int i;

	for (i = 0; i < suffix_len && i < len; i++)
		if (str[len - i - 1] != suffix[suffix_len - i - 1])
			return false;
	return i == suffix_len;
}

void assert(bool condition, int exit_status, char const *err_msg)
{
	if (!condition)
		die(err_msg, exit_status);
}

void die(char const *err, int status)
{
	write(2, err, ft_strlen(err));
	safe_exit(status);
}
