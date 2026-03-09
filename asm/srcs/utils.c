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
		die(exit_status, err_msg);
}

void die(int status, char const *err)
{
	write(2, err, ft_strlen(err));
	safe_exit(status);
}

char const *utils_strchr_set(char const *s, char const *set)
{
	int	i;
	int	j;

	if (!s)
		return (NULL);
	for (i = 0; s[i]; i++)
	{
		for (j = 0; set[j] && s[i] != set[j]; j++)
			;
		if (s[i] == set[j])
			return &s[i];
	}
	return (0);
}

uint32_t ft_htonl(uint32_t hostlong)
{
	uint32_t netlong;	
	uint8_t *host_ptr = (uint8_t *)&hostlong;
	uint8_t *net_ptr = (uint8_t *)&netlong;

	for (size_t i = 0; i < sizeof(hostlong); i++)
		net_ptr[i] = host_ptr[sizeof(hostlong) - 1 - i];
	return netlong;
}

uint32_t ft_htons(uint16_t hostlong)
{
	uint16_t netlong;	
	uint8_t *host_ptr = (uint8_t *)&hostlong;
	uint8_t *net_ptr = (uint8_t *)&netlong;

	for (size_t i = 0; i < sizeof(hostlong); i++)
		net_ptr[i] = host_ptr[sizeof(hostlong) - 1 - i];
	return netlong;
}
