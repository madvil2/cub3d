/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_map_utils_2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 18:58:16 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 22:12:48 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int	handle_empty_line_in_map(char **lines, int i, char *line)
{
	free(line);
	while (--i >= 0)
		free(lines[i]);
	free(lines);
	return (-1);
}

int	process_line(char **lines, int *i, char *line)
{
	if (is_empty_line(line))
	{
		if (*i > 0)
		{
			if (handle_empty_line_in_map(lines, *i, line) == -1)
				return (0);
		}
		else
			free(line);
	}
	else
	{
		lines[*i] = ft_strdup(line);
		(*i)++;
		free(line);
	}
	return (1);
}
