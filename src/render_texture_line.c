/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_texture_line.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	calculate_texture_close(t_tex_calc *calc)
{
	double	zoom_factor;
	double	visible_portion;
	double	player_height_ratio;
	double	center_of_tex;
	double	window_offset_ratio;

	zoom_factor = 1.0 / calc->perp_wall_dist;
	if (zoom_factor > 20.0)
		zoom_factor = 20.0;
	visible_portion = 1.0 / zoom_factor;
	player_height_ratio = 0.5;
	center_of_tex = calc->tex->height * player_height_ratio;
	*(calc->tex_pos) = center_of_tex - ((visible_portion * calc->tex->height) 
		* player_height_ratio);
	window_offset_ratio = (double)calc->draw_start / WINDOW_HEIGHT;
	*(calc->tex_pos) += window_offset_ratio * visible_portion * calc->tex->height;
	*(calc->step) = (visible_portion * calc->tex->height) / calc->line_height;
}

static void	calculate_texture_params(t_tex_params *params)
{
	t_tex_calc	calc;

	calc.perp_wall_dist = params->perp_wall_dist;
	calc.tex = params->tex;
	calc.step = params->step;
	calc.tex_pos = params->tex_pos;
	calc.line_height = params->line_height;
	calc.draw_start = params->draw_start;

	if (params->perp_wall_dist < 1.0)
		calculate_texture_close(&calc);
	else
	{
		*(params->step) = (double)params->tex->height / params->line_height;
		*(params->tex_pos) = 0;
	}
	*(params->tex_y) = (int)*(params->tex_pos);
	if (*(params->tex_y) >= params->tex->height)
		*(params->tex_y) = params->tex->height - 1;
	if (*(params->tex_y) < 0)
		*(params->tex_y) = 0;
}

static void	init_vertical_line_params(t_line_params *params, t_vert_line *line)
{
	int	tex_x;

	tex_x = line->tex_x;
	if (tex_x >= line->tex->width)
		tex_x = line->tex->width - 1;
	if (tex_x < 0)
		tex_x = 0;
	params->tex_x = tex_x;
	params->tex = line->tex;
	params->tex_data = mlx_get_data_addr(line->tex->img, &params->tex_bpp,
		&params->tex_line_size, &params->tex_endian);

	t_tex_params tex_params;
	int line_height = line->draw_end - line->draw_start;

	tex_params.tex_y = &params->tex_y;
	tex_params.tex_pos = &params->tex_pos;
	tex_params.step = &params->step;
	tex_params.tex = line->tex;
	tex_params.line_height = line_height;
	tex_params.draw_start = line->draw_start;
	tex_params.perp_wall_dist = line->perp_wall_dist;

	calculate_texture_params(&tex_params);
}

static void	render_vertical_line_pixels(t_game *game, t_vert_line *line, 
	t_line_params *params)
{
	int	y;
	int	color;

	y = line->draw_start;
	while (y < line->draw_end)
	{
		params->tex_y = (int)params->tex_pos;
		if (params->tex_y >= params->tex->height)
			params->tex_y = params->tex->height - 1;
		if (params->tex_y < 0)
			params->tex_y = 0;
		color = *(unsigned int *)(params->tex_data + (params->tex_y 
			* params->tex_line_size + params->tex_x * (params->tex_bpp / 8)));
		draw_pixel(&game->img, line->x, y, color);
		params->tex_pos += params->step;
		y++;
	}
}

void	draw_vertical_line(t_game *game, t_vert_line *line)
{
	t_line_params	params;

	init_vertical_line_params(&params, line);
	render_vertical_line_pixels(game, line, &params);
} 