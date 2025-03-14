/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycasting.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	prepare_ray_data(t_ray_data *data)
{
	double	camera_x;

	camera_x = 2 * data->x / (double)WINDOW_WIDTH - 1;
	*(data->ray_dir_x) = data->game->player.dir_x + data->game->player.plane_x * camera_x;
	*(data->ray_dir_y) = data->game->player.dir_y + data->game->player.plane_y * camera_x;
	*(data->delta_dist_x) = fabs(1 / *(data->ray_dir_x));
	*(data->delta_dist_y) = fabs(1 / *(data->ray_dir_y));
}

void	init_ray_data(t_game *game, int x, t_ray_params *ray, 
	t_ray_data *data)
{
	data->game = game;
	data->x = x;
	data->ray_dir_x = &ray->ray_dir_x;
	data->ray_dir_y = &ray->ray_dir_y;
	data->delta_dist_x = &ray->delta_dist_x;
	data->delta_dist_y = &ray->delta_dist_y;
	prepare_ray_data(data);
}

void	debug_ray(t_ray_params *ray, t_game *game, int x, int *debug_count)
{
	if ((*debug_count)++ % 60 == 0 && x == WINDOW_WIDTH / 2)
	{
		t_ray_debug ray_info;
		
		ray_info.ray_dir_x = ray->ray_dir_x;
		ray_info.ray_dir_y = ray->ray_dir_y;
		ray_info.plane_x = game->player.plane_x;
		ray_info.plane_y = game->player.plane_y;
		ray_info.camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
		
		debug_ray_info(&ray_info);
	}
}

void	cast_single_ray(t_game *game, int x, int *debug_count)
{
	t_ray_params	ray;
	t_ray_step		step;
	t_dda			dda;
	t_ray_data		data;
	t_ray_hit		hit;

	init_ray_data(game, x, &ray, &data);
	debug_ray(&ray, game, x, debug_count);
	init_ray_step(game, &ray, &step);
	init_dda(game, &ray, &dda);
	init_ray_hit(game, x, &ray, debug_count, &hit);
} 