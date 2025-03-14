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

static void	set_texture_by_direction(t_wall_hit *hit, t_game *game)
{
	if (hit->side == 0)
	{
		if (hit->ray_dir_x > 0)
			*(hit->tex) = &game->config.east;
		else
			*(hit->tex) = &game->config.west;
	}
	else
	{
		if (hit->ray_dir_y > 0)
			*(hit->tex) = &game->config.south;
		else
			*(hit->tex) = &game->config.north;
	}
}

void	calculate_wall_hit(t_game *game, t_wall_hit *hit)
{
	if (hit->side == 0)
		*(hit->wall_x) = game->player.y + hit->perp_wall_dist * hit->ray_dir_y;
	else
		*(hit->wall_x) = game->player.x + hit->perp_wall_dist * hit->ray_dir_x;
	
	set_texture_by_direction(hit, game);
	
	*(hit->wall_x) -= floor(*(hit->wall_x));
	*(hit->tex_x) = (int)(*(hit->wall_x) * (*(hit->tex))->width);
	if ((hit->side == 0 && hit->ray_dir_x > 0) || (hit->side == 1 && hit->ray_dir_y < 0))
		*(hit->tex_x) = (*(hit->tex))->width - *(hit->tex_x) - 1;
}

void	render_ceiling_floor(t_game *game)
{
	t_ceiling_floor	cf;

	cf.ceiling = game->config.ceiling;
	cf.floor = game->config.floor;
	cf.ceiling_color = (cf.ceiling.r << 16) | (cf.ceiling.g << 8) | cf.ceiling.b;
	cf.floor_color = (cf.floor.r << 16) | (cf.floor.g << 8) | cf.floor.b;
	
	cf.y = 0;
	while (cf.y < WINDOW_HEIGHT / 2)
	{
		cf.x = 0;
		while (cf.x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, cf.x, cf.y, cf.ceiling_color);
			cf.x++;
		}
		cf.y++;
	}
	while (cf.y < WINDOW_HEIGHT)
	{
		cf.x = 0;
		while (cf.x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, cf.x, cf.y, cf.floor_color);
			cf.x++;
		}
		cf.y++;
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