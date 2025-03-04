/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	debug_ray_info(double ray_dir_x, double ray_dir_y, 
	double plane_x, double plane_y, double camera_x)
{
	(void)ray_dir_x;
	(void)ray_dir_y;
	(void)plane_x;
	(void)plane_y;
	(void)camera_x;
}

void	debug_wall_hit(int map_x, int map_y, double perp_wall_dist,
	int line_height, int draw_start, int draw_end,
	double wall_x, int tex_x, t_texture *tex, int side)
{
	(void)map_x;
	(void)map_y;
	(void)perp_wall_dist;
	(void)line_height;
	(void)draw_start;
	(void)draw_end;
	(void)wall_x;
	(void)tex_x;
	(void)tex;
	(void)side;
}

void	debug_texture_info(t_game *game)
{
	(void)game;
} 