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

char	**collect_map_lines(char *first_line, int fd)
{
	char	**lines;
	char	*line;
	int		i;

	lines = malloc(sizeof(char *) * 1000);
	if (!lines)
		return (NULL);
	lines[0] = ft_strdup(first_line);
	i = 1;
	while ((line = get_next_line(fd)) != NULL)
	{
		if (!process_line(lines, &i, line))
			return (NULL);
	}
	lines[i] = NULL;
	return (lines);
}

int	alloc_map_grid(t_map *map)
{
	int	i;

	map->grid = malloc(sizeof(char *) * (map->height + 1));
	if (!map->grid)
		return (0);
	i = 0;
	while (i < map->height)
	{
		map->grid[i] = malloc(sizeof(char) * (map->width + 1));
		if (!map->grid[i])
		{
			while (--i >= 0)
				free(map->grid[i]);
			free(map->grid);
			return (0);
		}
		i++;
	}
	return (1);
}

void	copy_map_data(t_map *map, char **map_lines)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		ft_memset(map->grid[i], ' ', map->width);
		j = 0;
		while (map_lines[i][j] && map_lines[i][j] != '\n')
		{
			map->grid[i][j] = map_lines[i][j];
			j++;
		}
		map->grid[i][map->width] = '\0';
		i++;
	}
	map->grid[map->height] = NULL;
}

void	free_map_lines(char **map_lines)
{
	int	i;

	i = 0;
	while (map_lines[i])
	{
		free(map_lines[i]);
		i++;
	}
	free(map_lines);
}

int	get_map_dimensions(t_map *map, char **map_lines)
{
	int	i;
	int	j;
	int	max_width;

	map->height = 0;
	while (map_lines[map->height])
		map->height++;
	max_width = 0;
	i = 0;
	while (map_lines[i])
	{
		j = ft_strlen(map_lines[i]);
		if (j > max_width)
			max_width = j;
		i++;
	}
	map->width = max_width;
	return (1);
} 