/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycasting_hit.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	calculate_perp_wall_dist(t_ray_params *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist = (ray->side_dist_x - ray->delta_dist_x);
	else
		ray->perp_wall_dist = (ray->side_dist_y - ray->delta_dist_y);
}

void	init_wall_hit(t_ray_params *ray, t_wall_hit *wall_hit)
{
	wall_hit->ray_dir_x = ray->ray_dir_x;
	wall_hit->ray_dir_y = ray->ray_dir_y;
	wall_hit->perp_wall_dist = ray->perp_wall_dist;
	wall_hit->side = ray->side;
	wall_hit->wall_x = &ray->wall_x;
	wall_hit->tex = &ray->tex;
	wall_hit->tex_x = &ray->tex_x;
}

void	create_wall_debug(t_ray_params *ray, t_wall_debug *wall_info)
{
	wall_info->map_x = ray->map_x;
	wall_info->map_y = ray->map_y;
	wall_info->perp_wall_dist = ray->perp_wall_dist;
	wall_info->line_height = ray->line_height;
	wall_info->draw_start = ray->draw_start;
	wall_info->draw_end = ray->draw_end;
	wall_info->wall_x = ray->wall_x;
	wall_info->tex_x = ray->tex_x;
	wall_info->tex = ray->tex;
	wall_info->side = ray->side;
}

void	process_ray_hit(t_ray_hit *hit)
{
	t_ray_params	*ray;
	t_wall_hit		wall_hit;

	ray = hit->ray;
	calculate_perp_wall_dist(ray);
	calculate_wall_drawing(ray->perp_wall_dist, &ray->line_height, 
		&ray->draw_start, &ray->draw_end);
	
	init_wall_hit(ray, &wall_hit);
	calculate_wall_hit(hit->game, &wall_hit);
	
	if (*(hit->debug_count) % 60 == 1 && hit->x == WINDOW_WIDTH / 2)
	{
		t_wall_debug wall_info;
		create_wall_debug(ray, &wall_info);
		debug_wall_hit(&wall_info);
	}
	
	draw_vertical_line(hit->game, hit->x, ray->draw_start, ray->draw_end, 
		ray->tex_x, ray->tex, ray->perp_wall_dist);
}

void	init_ray_hit(t_game *game, int x, t_ray_params *ray, 
	int *debug_count, t_ray_hit *hit)
{
	hit->game = game;
	hit->ray = ray;
	hit->x = x;
	hit->debug_count = debug_count;
	process_ray_hit(hit);
}
