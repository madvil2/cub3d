/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_minimap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

# define MAP_SCALE 16
# define PLAYER_SIZE 4

static void	draw_player_position(t_game *game, int center_x, int center_y)
{
	int	i;
	int	j;

	i = -PLAYER_SIZE / 2;
	while (i <= PLAYER_SIZE / 2)
	{
		j = -PLAYER_SIZE / 2;
		while (j <= PLAYER_SIZE / 2)
		{
			draw_pixel(&game->img, center_x + i, center_y + j, 0xFF0000);
			j++;
		}
		i++;
	}
}

static void	draw_player_direction(t_game *game, int center_x, int center_y)
{
	int		dir_x;
	int		dir_y;
	double	dx;
	double	dy;
	double	steps;
	double	x_inc;
	double	y_inc;
	double	x;
	double	y;
	int		i;

	dir_x = center_x + (int)(game->player.dir_x * MAP_SCALE);
	dir_y = center_y + (int)(game->player.dir_y * MAP_SCALE);
	dx = dir_x - center_x;
	dy = dir_y - center_y;
	steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
	x_inc = dx / steps;
	y_inc = dy / steps;
	x = center_x;
	y = center_y;
	i = 0;
	while (i <= steps)
	{
		draw_pixel(&game->img, (int)x, (int)y, 0xFF0000);
		x += x_inc;
		y += y_inc;
		i++;
	}
}

static void	draw_player(t_game *game, int map_x, int map_y)
{
	int	center_x;
	int	center_y;

	center_x = map_x + (int)(game->player.x * MAP_SCALE);
	center_y = map_y + (int)(game->player.y * MAP_SCALE);
	draw_player_position(game, center_x, center_y);
	draw_player_direction(game, center_x, center_y);
}

static void	swap_pixels(t_game *game, int x1, int y1, int x2, int y2)
{
	unsigned int	left_color;
	unsigned int	right_color;
	char			*left_addr;
	char			*right_addr;
	int				bytes_per_pixel;

	bytes_per_pixel = game->img.bits_per_pixel / 8;
	left_addr = game->img.addr + (y1 * game->img.line_length + 
		x1 * bytes_per_pixel);
	right_addr = game->img.addr + (y2 * game->img.line_length + 
		x2 * bytes_per_pixel);
	
	left_color = *(unsigned int *)left_addr;
	right_color = *(unsigned int *)right_addr;
	
	draw_pixel(&game->img, x1, y1, right_color);
	draw_pixel(&game->img, x2, y2, left_color);
}

static void	flip_minimap(t_game *game, int map_x, int map_y, int w, int h)
{
	int	x;
	int	y;
	int	half_width;

	half_width = w / 2;
	y = 0;
	while (y < h)
	{
		x = 0;
		while (x < half_width)
		{
			swap_pixels(game, x + map_x, y + map_y, 
				w - 1 - x + map_x, y + map_y);
			x++;
		}
		y++;
	}
}

static int	get_cell_color(t_game *game, int x, int y)
{
	if (game->config.map.grid[y][x] == '1')
		return (0xFFFFFF);
	else if (game->config.map.grid[y][x] == '0' || 
			game->config.map.grid[y][x] == 'N' ||
			game->config.map.grid[y][x] == 'S' ||
			game->config.map.grid[y][x] == 'E' ||
			game->config.map.grid[y][x] == 'W')
		return (0x404040);
	else
		return (0x000000);
}

void	render_map(t_game *game)
{
	int	x;
	int	y;
	int	map_x;
	int	map_y;
	int	map_width;
	int	map_height;

	map_x = 10;
	map_y = 10;
	map_width = game->config.map.width * MAP_SCALE;
	map_height = game->config.map.height * MAP_SCALE;
	y = 0;
	while (y < game->config.map.height)
	{
		x = 0;
		while (x < game->config.map.width)
		{
			draw_rectangle(&game->img, map_x + x * MAP_SCALE, 
				map_y + y * MAP_SCALE, MAP_SCALE - 1, 
				get_cell_color(game, x, y));
			x++;
		}
		y++;
	}
	draw_player(game, map_x, map_y);
	flip_minimap(game, map_x, map_y, map_width, map_height);
} 