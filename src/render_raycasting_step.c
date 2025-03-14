/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycasting_step.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	calculate_ray_step_x(t_ray_step *step, int *map_coord)
{
	if (step->ray_dir_x < 0)
	{
		*(step->step_x) = -1;
		*(step->side_dist_x) = (step->game->player.x - map_coord[0]) 
			* fabs(1 / step->ray_dir_x);
	}
	else
	{
		*(step->step_x) = 1;
		*(step->side_dist_x) = (map_coord[0] + 1.0 - step->game->player.x) 
			* fabs(1 / step->ray_dir_x);
	}
}

static void	calculate_ray_step_y(t_ray_step *step, int *map_coord)
{
	if (step->ray_dir_y < 0)
	{
		*(step->step_y) = -1;
		*(step->side_dist_y) = (step->game->player.y - map_coord[1]) 
			* fabs(1 / step->ray_dir_y);
	}
	else
	{
		*(step->step_y) = 1;
		*(step->side_dist_y) = (map_coord[1] + 1.0 - step->game->player.y)
			* fabs(1 / step->ray_dir_y);
	}
}

void	calculate_ray_step(t_ray_step *step)
{
	int	map_coord[2];

	map_coord[0] = (int)step->game->player.x;
	map_coord[1] = (int)step->game->player.y;
	calculate_ray_step_x(step, map_coord);
	calculate_ray_step_y(step, map_coord);
}

void	perform_dda(t_dda *dda)
{
	int	hit;

	hit = 0;
	*(dda->map_x) = (int)dda->game->player.x;
	*(dda->map_y) = (int)dda->game->player.y;
	while (hit == 0)
	{
		if (*(dda->side_dist_x) < *(dda->side_dist_y))
		{
			*(dda->side_dist_x) += dda->delta_dist_x;
			*(dda->map_x) += dda->step_x;
			*(dda->side) = 0;
		}
		else
		{
			*(dda->side_dist_y) += dda->delta_dist_y;
			*(dda->map_y) += dda->step_y;
			*(dda->side) = 1;
		}
		if (dda->game->config.map.grid[*(dda->map_y)][*(dda->map_x)] == '1')
			hit = 1;
	}
}

void	init_dda(t_game *game, t_ray_params *ray, t_dda *dda)
{
	dda->game = game;
	dda->map_x = &ray->map_x;
	dda->map_y = &ray->map_y;
	dda->step_x = ray->step_x;
	dda->step_y = ray->step_y;
	dda->side_dist_x = &ray->side_dist_x;
	dda->side_dist_y = &ray->side_dist_y;
	dda->delta_dist_x = ray->delta_dist_x;
	dda->delta_dist_y = ray->delta_dist_y;
	dda->side = &ray->side;
	perform_dda(dda);
}
