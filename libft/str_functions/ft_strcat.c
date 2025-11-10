/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhubleur <dhubleur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 15:28:18 by dhubleur          #+#    #+#             */
/*   Updated: 2023/11/24 15:28:23 by dhubleur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strcat(char *dest, char *src)
{
	char *dest_start;

	dest_start = dest;
	while (*dest)
		dest++;
	while (*src)
		*dest++ = *src++;
	*dest = 0;
	return (dest_start);
}