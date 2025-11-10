/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhubleur <dhubleur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/23 17:17:59 by dhubleur          #+#    #+#             */
/*   Updated: 2021/11/24 11:51:29 by dhubleur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

list_t	*ft_lstnew(void *content)
{
	list_t	*obj;

	obj = malloc(sizeof(list_t));
	if (!obj)
		return (NULL);
	obj->content = content;
	obj->next = NULL;
	return (obj);
}
