/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_walls.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	calculate_wall_drawing(double perp_wall_dist, int *line_height, 
	int *draw_start, int *draw_end)
{
	if (perp_wall_dist <= 0.1)
		perp_wall_dist = 0.1;
	*line_height = (int)(WINDOW_HEIGHT / perp_wall_dist);
	*draw_start = -(*line_height) / 2 + WINDOW_HEIGHT / 2;
	if (*draw_start < 0)
		*draw_start = 0;
	*draw_end = (*line_height) / 2 + WINDOW_HEIGHT / 2;
	if (*draw_end >= WINDOW_HEIGHT)
		*draw_end = WINDOW_HEIGHT - 1;
}

static void	set_texture_by_direction(double ray_dir_x, double ray_dir_y,
	int side, t_texture **tex, t_game *game)
{
	if (side == 0)
	{
		if (ray_dir_x > 0)
			*tex = &game->config.east;
		else
			*tex = &game->config.west;
	}
	else
	{
		if (ray_dir_y > 0)
			*tex = &game->config.south;
		else
			*tex = &game->config.north;
	}
}

void	calculate_wall_hit(t_game *game, double ray_dir_x, 
	double ray_dir_y, double perp_wall_dist, int side, 
	double *wall_x, t_texture **tex, int *tex_x)
{
	if (side == 0)
		*wall_x = game->player.y + perp_wall_dist * ray_dir_y;
	else
		*wall_x = game->player.x + perp_wall_dist * ray_dir_x;
	
	set_texture_by_direction(ray_dir_x, ray_dir_y, side, tex, game);
	
	*wall_x -= floor(*wall_x);
	*tex_x = (int)(*wall_x * (*tex)->width);
	if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0))
		*tex_x = (*tex)->width - *tex_x - 1;
}

void	render_ceiling_floor(t_game *game)
{
	int	x;
	int	y;
	int	ceiling_color;
	int	floor_color;
	t_color ceiling;
	t_color floor;

	ceiling = game->config.ceiling;
	floor = game->config.floor;
	ceiling_color = (ceiling.r << 16) | (ceiling.g << 8) | ceiling.b;
	floor_color = (floor.r << 16) | (floor.g << 8) | floor.b;
	
	y = 0;
	while (y < WINDOW_HEIGHT / 2)
	{
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, x, y, ceiling_color);
			x++;
		}
		y++;
	}
	while (y < WINDOW_HEIGHT)
	{
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, x, y, floor_color);
			x++;
		}
		y++;
	}
}

void	render_walls(t_game *game)
{
	int			x;
	static int	debug_count = 0;

	x = 0;
	while (x < WINDOW_WIDTH)
	{
		cast_single_ray(game, x, &debug_count);
		x++;
	}
} 