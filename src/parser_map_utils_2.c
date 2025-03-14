/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_map_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
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
