/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_map_utils_1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 18:58:16 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 21:19:28 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

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
	line = get_next_line(fd);
	while (!line)
	{
		if (!process_line(lines, &i, line))
			return (NULL);
		line = get_next_line(fd);
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
