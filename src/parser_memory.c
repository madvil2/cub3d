/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_memory.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	free_map_grid(t_map *map)
{
	int	i;

	if (!map->grid)
		return ;
	i = 0;
	while (i < map->height)
	{
		free(map->grid[i]);
		i++;
	}
	free(map->grid);
}

void	free_config(t_config *config)
{
	if (config->north.path)
		free(config->north.path);
	if (config->south.path)
		free(config->south.path);
	if (config->west.path)
		free(config->west.path);
	if (config->east.path)
		free(config->east.path);
	free_map_grid(&config->map);
} 