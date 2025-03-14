/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:43:20 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/04 18:58:47 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	init_player_direction(t_game *game)
{
	if (game->config.map.player_dir == 'N')
	{
		game->player.dir_x = 0;
		game->player.dir_y = -1;
	}
	else if (game->config.map.player_dir == 'S')
	{
		game->player.dir_x = 0;
		game->player.dir_y = 1;
	}
	else if (game->config.map.player_dir == 'E')
	{
		game->player.dir_x = 1;
		game->player.dir_y = 0;
	}
	else
	{
		game->player.dir_x = -1;
		game->player.dir_y = 0;
	}
	game->player.plane_x = game->player.dir_y * 0.66;
	game->player.plane_y = -game->player.dir_x * 0.66;
}

int	init_game(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		return (0);
	game->win = mlx_new_window(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
	if (!game->win)
		return (0);
	game->img.img = mlx_new_image(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!game->img.img)
		return (0);
	game->img.addr = mlx_get_data_addr(game->img.img, &game->img.bits_per_pixel,
			&game->img.line_length, &game->img.endian);
	game->mouse_control = 0;
	game->last_mouse_x = WINDOW_WIDTH / 2;
	load_textures(game);
	game->player.x = game->config.map.player_x + 0.5;
	game->player.y = game->config.map.player_y + 0.5;
	init_player_direction(game);
	return (1);
}

void	cleanup_game(t_game *game)
{
	if (game->config.north.img)
		mlx_destroy_image(game->mlx, game->config.north.img);
	if (game->config.south.img)
		mlx_destroy_image(game->mlx, game->config.south.img);
	if (game->config.west.img)
		mlx_destroy_image(game->mlx, game->config.west.img);
	if (game->config.east.img)
		mlx_destroy_image(game->mlx, game->config.east.img);
	if (game->img.img)
		mlx_destroy_image(game->mlx, game->img.img);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	free_config(&game->config);
	if (game->mlx)
		free(game->mlx);
	exit(0);
}

int	handle_close(t_game *game)
{
	cleanup_game(game);
	return (0);
}
