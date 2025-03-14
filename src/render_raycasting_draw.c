/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycasting_draw.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:26:40 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 22:14:37 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

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

void	render_ray_hit(t_ray_hit *hit)
{
	t_ray_params	*ray;
	t_vert_line		line;

	ray = hit->ray;
	line.x = hit->x;
	line.draw_start = ray->draw_start;
	line.draw_end = ray->draw_end;
	line.tex_x = ray->tex_x;
	line.tex = ray->tex;
	line.perp_wall_dist = ray->perp_wall_dist;
	draw_vertical_line(hit->game, &line);
}
