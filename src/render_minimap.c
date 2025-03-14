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

static void	draw_line(t_img *img, t_line *line)
{
	double	dx;
	double	dy;
	double	steps;
	double	x_inc;
	double	y_inc;

	dx = line->end_x - line->start_x;
	dy = line->end_y - line->start_y;
	steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
	x_inc = dx / steps;
	y_inc = dy / steps;
	while (steps >= 0)
	{
		draw_pixel(img, (int)line->start_x, (int)line->start_y, line->color);
		line->start_x += x_inc;
		line->start_y += y_inc;
		steps--;
	}
}

static void	draw_player_direction(t_game *game, int center_x, int center_y)
{
	t_line	line;
	int		dir_x;
	int		dir_y;

	dir_x = center_x + (int)(game->player.dir_x * MAP_SCALE);
	dir_y = center_y + (int)(game->player.dir_y * MAP_SCALE);
	line.start_x = center_x;
	line.start_y = center_y;
	line.end_x = dir_x;
	line.end_y = dir_y;
	line.color = 0xFF0000;
	draw_line(&game->img, &line);
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

static void	swap_pixels(t_game *game, t_pixel p1, t_pixel p2)
{
	unsigned int	left_color;
	unsigned int	right_color;
	char			*left_addr;
	char			*right_addr;
	int				bytes_per_pixel;

	bytes_per_pixel = game->img.bits_per_pixel / 8;
	left_addr = game->img.addr + (p1.y * game->img.line_length + 
		p1.x * bytes_per_pixel);
	right_addr = game->img.addr + (p2.y * game->img.line_length + 
		p2.x * bytes_per_pixel);
	
	left_color = *(unsigned int *)left_addr;
	right_color = *(unsigned int *)right_addr;
	
	draw_pixel(&game->img, p1.x, p1.y, right_color);
	draw_pixel(&game->img, p2.x, p2.y, left_color);
}

static void	flip_minimap(t_game *game, t_minimap map)
{
	int		x;
	int		y;
	int		half_width;
	t_pixel	p1;
	t_pixel	p2;

	half_width = map.width / 2;
	y = 0;
	while (y < map.height)
	{
		x = 0;
		while (x < half_width)
		{
			p1.x = x + map.x;
			p1.y = y + map.y;
			p2.x = map.width - 1 - x + map.x;
			p2.y = y + map.y;
			swap_pixels(game, p1, p2);
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
	int			x;
	int			y;
	int			map_x;
	int			map_y;
	t_minimap	minimap;
	t_rect		rect;

	map_x = 10;
	map_y = 10;
	minimap.x = map_x;
	minimap.y = map_y;
	minimap.width = game->config.map.width * MAP_SCALE;
	minimap.height = game->config.map.height * MAP_SCALE;
	y = 0;
	while (y < game->config.map.height)
	{
		x = 0;
		while (x < game->config.map.width)
		{
			rect.x = map_x + x * MAP_SCALE;
			rect.y = map_y + y * MAP_SCALE;
			rect.size = MAP_SCALE - 1;
			rect.color = get_cell_color(game, x, y);
			draw_rectangle(&game->img, rect);
			x++;
		}
		y++;
	}
	draw_player(game, map_x, map_y);
	flip_minimap(game, minimap);
} 