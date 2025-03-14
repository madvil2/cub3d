/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_validate.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	is_valid_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S'
		|| c == 'E' || c == 'W' || c == ' ' || c == '\n');
}

static int	check_wall_surroundings(t_map *map, int i, int j)
{
	if (i == 0 || i == map->height - 1 || j == 0 || j == map->width - 1
		|| map->grid[i - 1][j] == ' ' || map->grid[i + 1][j] == ' '
		|| map->grid[i][j - 1] == ' ' || map->grid[i][j + 1] == ' ')
		return (0);
	return (1);
}

static int	validate_map_walls(t_map *map)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width && map->grid[i][j])
		{
			if (map->grid[i][j] == '0' || map->grid[i][j] == 'N'
				|| map->grid[i][j] == 'S' || map->grid[i][j] == 'E'
				|| map->grid[i][j] == 'W')
			{
				if (!check_wall_surroundings(map, i, j))
					return (0);
			}
			j++;
		}
		i++;
	}
	return (1);
}

static int	validate_map_chars(t_map *map, int *player_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width && map->grid[i][j])
		{
			if (!is_valid_map_char(map->grid[i][j]))
				return (0);
			if (map->grid[i][j] == 'N' || map->grid[i][j] == 'S'
				|| map->grid[i][j] == 'E' || map->grid[i][j] == 'W')
			{
				(*player_count)++;
				map->player_x = j;
				map->player_y = i;
				map->player_dir = map->grid[i][j];
			}
			j++;
		}
		i++;
	}
	return (1);
}

int	validate_map(t_map *map)
{
	int	player_count;

	player_count = 0;
	if (!validate_map_chars(map, &player_count))
		return (0);
	if (!validate_map_walls(map))
		return (0);
	return (player_count == 1);
}
