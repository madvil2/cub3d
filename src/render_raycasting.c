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

static void	prepare_ray_data(t_game *game, int x, double *ray_dir_x,
	double *ray_dir_y, double *delta_dist_x, double *delta_dist_y)
{
	double	camera_x;

	camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
	*ray_dir_x = game->player.dir_x + game->player.plane_x * camera_x;
	*ray_dir_y = game->player.dir_y + game->player.plane_y * camera_x;
	*delta_dist_x = fabs(1 / *ray_dir_x);
	*delta_dist_y = fabs(1 / *ray_dir_y);
}

static void	calculate_ray_step(t_game *game, double ray_dir_x, 
	double ray_dir_y, int *step_x, int *step_y, 
	double *side_dist_x, double *side_dist_y)
{
	int	map_x;
	int	map_y;

	map_x = (int)game->player.x;
	map_y = (int)game->player.y;
	if (ray_dir_x < 0)
	{
		*step_x = -1;
		*side_dist_x = (game->player.x - map_x) * fabs(1 / ray_dir_x);
	}
	else
	{
		*step_x = 1;
		*side_dist_x = (map_x + 1.0 - game->player.x) * fabs(1 / ray_dir_x);
	}
	if (ray_dir_y < 0)
	{
		*step_y = -1;
		*side_dist_y = (game->player.y - map_y) * fabs(1 / ray_dir_y);
	}
	else
	{
		*step_y = 1;
		*side_dist_y = (map_y + 1.0 - game->player.y) * fabs(1 / ray_dir_y);
	}
}

static void	perform_dda(t_game *game, int *map_x, int *map_y, 
	int step_x, int step_y, double *side_dist_x, 
	double *side_dist_y, double delta_dist_x, 
	double delta_dist_y, int *side)
{
	int	hit;

	hit = 0;
	*map_x = (int)game->player.x;
	*map_y = (int)game->player.y;
	while (hit == 0)
	{
		if (*side_dist_x < *side_dist_y)
		{
			*side_dist_x += delta_dist_x;
			*map_x += step_x;
			*side = 0;
		}
		else
		{
			*side_dist_y += delta_dist_y;
			*map_y += step_y;
			*side = 1;
		}
		if (game->config.map.grid[*map_y][*map_x] == '1')
			hit = 1;
	}
}

static void	process_ray_hit(t_game *game, int x, t_ray_params *ray, 
	int *debug_count)
{
	if (ray->side == 0)
		ray->perp_wall_dist = (ray->side_dist_x - ray->delta_dist_x);
	else
		ray->perp_wall_dist = (ray->side_dist_y - ray->delta_dist_y);
	
	calculate_wall_drawing(ray->perp_wall_dist, &ray->line_height, 
		&ray->draw_start, &ray->draw_end);
	
	calculate_wall_hit(game, ray->ray_dir_x, ray->ray_dir_y, 
		ray->perp_wall_dist, ray->side, &ray->wall_x, 
		&ray->tex, &ray->tex_x);
	
	if (*debug_count % 60 == 1 && x == WINDOW_WIDTH / 2)
		debug_wall_hit(ray->map_x, ray->map_y, ray->perp_wall_dist, 
			ray->line_height, ray->draw_start, ray->draw_end, 
			ray->wall_x, ray->tex_x, ray->tex, ray->side);
	
	draw_vertical_line(game, x, ray->draw_start, ray->draw_end, 
		ray->tex_x, ray->tex, ray->perp_wall_dist);
}

void	cast_single_ray(t_game *game, int x, int *debug_count)
{
	t_ray_params	ray;

	prepare_ray_data(game, x, &ray.ray_dir_x, &ray.ray_dir_y, 
		&ray.delta_dist_x, &ray.delta_dist_y);
	
	if ((*debug_count)++ % 60 == 0 && x == WINDOW_WIDTH / 2)
		debug_ray_info(ray.ray_dir_x, ray.ray_dir_y, game->player.plane_x, 
			game->player.plane_y, 2 * x / (double)WINDOW_WIDTH - 1);
	
	calculate_ray_step(game, ray.ray_dir_x, ray.ray_dir_y, &ray.step_x, 
		&ray.step_y, &ray.side_dist_x, &ray.side_dist_y);
	
	perform_dda(game, &ray.map_x, &ray.map_y, ray.step_x, ray.step_y, 
		&ray.side_dist_x, &ray.side_dist_y, ray.delta_dist_x, 
		ray.delta_dist_y, &ray.side);
	
	process_ray_hit(game, x, &ray, debug_count);
} 