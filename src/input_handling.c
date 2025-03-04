/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:43:20 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/04 18:58:47 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define MOUSE_SENSITIVITY 0.002
#define ROT_SPEED 0.05

void	handle_mouse_toggle(t_game *game)
{
	game->mouse_control = !game->mouse_control;
	if (game->mouse_control)
		mlx_mouse_hide();
	else
		mlx_mouse_show();
}

int	handle_mouse_move(int x, int y, t_game *game)
{
	static int	last_x = -1;
	double		rotation;

	(void)y;
	if (!game->mouse_control)
		return (0);
	if (last_x == -1)
	{
		last_x = x;
		return (0);
	}
	rotation = (x - last_x) * MOUSE_SENSITIVITY;
	last_x = x;
	handle_player_rotation(game, rotation);
	if (x < 100 || x > WINDOW_WIDTH - 100)
	{
		mlx_mouse_move(game->win, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		last_x = WINDOW_WIDTH / 2;
	}
	return (0);
}

int	handle_keypress(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		cleanup_game(game);
	else if (keycode == KEY_M)
		handle_mouse_toggle(game);
	else if (keycode == KEY_W)
		handle_player_movement(game, game->player.dir_x, game->player.dir_y);
	else if (keycode == KEY_S)
		handle_player_movement(game, -game->player.dir_x, -game->player.dir_y);
	else if (keycode == KEY_D)
		handle_player_movement(game, -game->player.plane_x, -game->player.plane_y);
	else if (keycode == KEY_A)
		handle_player_movement(game, game->player.plane_x, game->player.plane_y);
	else if (keycode == KEY_RIGHT)
		handle_player_rotation(game, ROT_SPEED);
	else if (keycode == KEY_LEFT)
		handle_player_rotation(game, -ROT_SPEED);
	return (0);
} 