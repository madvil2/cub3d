/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_movement.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:43:20 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/04 18:58:47 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define MOVE_SPEED 0.05
#define ROT_SPEED 0.05
#define WALL_OFFSET 0.2

int	check_collision(t_game *game, double new_x, double new_y)
{
	int	map_x;
	int	map_y;

	map_x = (int)new_x;
	map_y = (int)new_y;
	if (game->config.map.grid[map_y][map_x] == '1')
		return (1);
	if (game->config.map.grid[(int)(new_y + WALL_OFFSET)]
		[(int)(new_x + WALL_OFFSET)] == '1'
		|| game->config.map.grid[(int)(new_y + WALL_OFFSET)]
		[(int)(new_x - WALL_OFFSET)] == '1'
		|| game->config.map.grid[(int)(new_y - WALL_OFFSET)]
		[(int)(new_x + WALL_OFFSET)] == '1'
		|| game->config.map.grid[(int)(new_y - WALL_OFFSET)]
		[(int)(new_x - WALL_OFFSET)] == '1')
		return (1);
	return (0);
}

void	handle_player_rotation(t_game *game, double rotation)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = game->player.dir_x;
	game->player.dir_x = game->player.dir_x * cos(rotation)
		- game->player.dir_y * sin(rotation);
	game->player.dir_y = old_dir_x * sin(rotation)
		+ game->player.dir_y * cos(rotation);
	old_plane_x = game->player.plane_x;
	game->player.plane_x = game->player.plane_x * cos(rotation)
		- game->player.plane_y * sin(rotation);
	game->player.plane_y = old_plane_x * sin(rotation)
		+ game->player.plane_y * cos(rotation);
}

void	handle_player_movement(t_game *game, double dir_x, double dir_y)
{
	double	new_x;
	double	new_y;

	new_x = game->player.x + dir_x * MOVE_SPEED;
	new_y = game->player.y + dir_y * MOVE_SPEED;
	if (!check_collision(game, new_x, new_y))
	{
		game->player.x = new_x;
		game->player.y = new_y;
	}
}
