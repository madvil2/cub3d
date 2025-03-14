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

static void	init_minimap(t_game *game, t_minimap *minimap)
{
	minimap->x = 10;
	minimap->y = 10;
	minimap->width = game->config.map.width * MAP_SCALE;
	minimap->height = game->config.map.height * MAP_SCALE;
}

static void	draw_map_cells(t_game *game, int map_x, int map_y)
{
	int		x;
	int		y;
	t_rect	rect;

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
}

void	render_map(t_game *game)
{
	t_minimap	minimap;
	int			map_x;
	int			map_y;

	map_x = 10;
	map_y = 10;
	init_minimap(game, &minimap);
	draw_map_cells(game, map_x, map_y);
	draw_player(game, map_x, map_y);
	flip_minimap(game, minimap);
} 