/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:43:20 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/04 18:58:47 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	game_loop(t_game *game)
{
	render_frame(game);
	return (0);
}

static int	validate_args(int argc)
{
	if (argc != 2)
	{
		ft_putstr_fd(ERR_ARGS, 2);
		ft_putstr_fd("\n", 2);
		return (0);
	}
	return (1);
}

static int	init_config_and_parse(t_game *game, char *map_path)
{
	ft_memset(&game->config, 0, sizeof(t_config));
	game->config.floor.r = -1;
	game->config.ceiling.r = -1;
	if (!parse_config(game, map_path))
	{
		ft_putstr_fd(ERR_FORMAT, 2);
		ft_putstr_fd("\n", 2);
		return (0);
	}
	return (1);
}

static void	setup_hooks(t_game *game)
{
	mlx_hook(game->win, 2, 1L << 0, handle_keypress, game);
	mlx_hook(game->win, 6, 1L << 6, handle_mouse_move, game);
	mlx_hook(game->win, 17, 0, handle_close, game);
	mlx_mouse_move(game->win, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	mlx_loop_hook(game->mlx, game_loop, game);
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (!validate_args(argc))
		return (1);
	if (!init_config_and_parse(&game, argv[1]))
		return (1);
	if (!init_game(&game))
	{
		ft_putstr_fd("Error\nFailed to initialize game\n", 2);
		return (1);
	}
	setup_hooks(&game);
	mlx_loop(game.mlx);
	return (0);
} 