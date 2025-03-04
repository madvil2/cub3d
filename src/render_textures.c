/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_textures.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	load_texture(t_game *game, t_texture *texture)
{
	int	width;
	int	height;

	texture->img = mlx_xpm_file_to_image(game->mlx, texture->path, 
		&width, &height);
	if (!texture->img)
		return ;
	texture->width = width;
	texture->height = height;
}

void	load_textures(t_game *game)
{
	load_texture(game, &game->config.north);
	load_texture(game, &game->config.south);
	load_texture(game, &game->config.east);
	load_texture(game, &game->config.west);
}

static void	calculate_texture_close(double perp_wall_dist, t_texture *tex, 
	double *step, double *tex_pos, int line_height, int draw_start)
{
	double	zoom_factor;
	double	visible_portion;
	double	player_height_ratio;
	double	center_of_tex;
	double	window_offset_ratio;

	zoom_factor = 1.0 / perp_wall_dist;
	if (zoom_factor > 20.0)
		zoom_factor = 20.0;
	visible_portion = 1.0 / zoom_factor;
	player_height_ratio = 0.5;
	center_of_tex = tex->height * player_height_ratio;
	*tex_pos = center_of_tex - ((visible_portion * tex->height) 
		* player_height_ratio);
	window_offset_ratio = (double)draw_start / WINDOW_HEIGHT;
	*tex_pos += window_offset_ratio * visible_portion * tex->height;
	*step = (visible_portion * tex->height) / line_height;
}

static void	calculate_texture_params(int *tex_y, double *tex_pos, 
	double *step, t_texture *tex, int line_height, 
	int draw_start, double perp_wall_dist)
{
	if (perp_wall_dist < 1.0)
		calculate_texture_close(perp_wall_dist, tex, step, tex_pos, 
			line_height, draw_start);
	else
	{
		*step = (double)tex->height / line_height;
		*tex_pos = 0;
	}
	*tex_y = (int)*tex_pos;
	if (*tex_y >= tex->height)
		*tex_y = tex->height - 1;
	if (*tex_y < 0)
		*tex_y = 0;
}

static void	init_vertical_line_params(int tex_x, t_texture *tex,
	double perp_wall_dist, int draw_start, int line_height,
	t_line_params *params)
{
	if (tex_x >= tex->width)
		tex_x = tex->width - 1;
	if (tex_x < 0)
		tex_x = 0;
	params->tex_x = tex_x;
	params->tex = tex;
	params->tex_data = mlx_get_data_addr(tex->img, &params->tex_bpp,
		&params->tex_line_size, &params->tex_endian);
	calculate_texture_params(&params->tex_y, &params->tex_pos, &params->step,
		tex, line_height, draw_start, perp_wall_dist);
}

void	draw_vertical_line(t_game *game, int x, int draw_start, 
	int draw_end, int tex_x, t_texture *tex, double perp_wall_dist)
{
	int				y;
	int				color;
	int				line_height;
	t_line_params	params;

	line_height = draw_end - draw_start;
	init_vertical_line_params(tex_x, tex, perp_wall_dist, 
		draw_start, line_height, &params);
	y = draw_start;
	while (y < draw_end)
	{
		params.tex_y = (int)params.tex_pos;
		if (params.tex_y >= params.tex->height)
			params.tex_y = params.tex->height - 1;
		if (params.tex_y < 0)
			params.tex_y = 0;
		color = *(unsigned int *)(params.tex_data + (params.tex_y 
			* params.tex_line_size + params.tex_x * (params.tex_bpp / 8)));
		draw_pixel(&game->img, x, y, color);
		params.tex_pos += params.step;
		y++;
	}
} 