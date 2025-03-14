/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_memory.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:19:07 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 21:53:38 by kokaimov         ###   ########.fr       */
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
