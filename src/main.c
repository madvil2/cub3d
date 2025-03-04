/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
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
#define MOUSE_SENSITIVITY 0.002

static void	init_player_direction(t_game *game);
static int	check_collision(t_game *game, double new_x, double new_y);
static void	handle_player_rotation(t_game *game, double rotation);
static void	handle_player_movement(t_game *game, double dir_x, double dir_y);
static void	handle_mouse_toggle(t_game *game);

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

static int	check_collision(t_game *game, double new_x, double new_y)
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

static void	handle_player_rotation(t_game *game, double rotation)
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

static void	handle_player_movement(t_game *game, double dir_x, double dir_y)
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

static void	handle_mouse_toggle(t_game *game)
{
	game->mouse_control = !game->mouse_control;
	if (game->mouse_control)
		mlx_mouse_hide();
	else
		mlx_mouse_show();
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

int	game_loop(t_game *game)
{
	render_frame(game);
	return (0);
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
	{
		ft_putstr_fd(ERR_ARGS, 2);
		ft_putstr_fd("\n", 2);
		return (1);
	}
	ft_memset(&game.config, 0, sizeof(t_config));
	game.config.floor.r = -1;
	game.config.ceiling.r = -1;
	if (!parse_config(&game, argv[1]))
	{
		ft_putstr_fd(ERR_FORMAT, 2);
		ft_putstr_fd("\n", 2);
		return (1);
	}
	if (!init_game(&game))
	{
		ft_putstr_fd("Error\nFailed to initialize game\n", 2);
		return (1);
	}
	mlx_hook(game.win, 2, 1L << 0, handle_keypress, &game);
	mlx_hook(game.win, 6, 1L << 6, handle_mouse_move, &game);
	mlx_hook(game.win, 17, 0, handle_close, &game);
	mlx_mouse_move(game.win, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
	return (0);
} 