/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_ceiling_floor.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	init_ceiling_floor_colors(t_game *game, t_ceiling_floor *cf)
{
	cf->ceiling = game->config.ceiling;
	cf->floor = game->config.floor;
	cf->ceiling_color = (cf->ceiling.r << 16) | (cf->ceiling.g << 8) | cf->ceiling.b;
	cf->floor_color = (cf->floor.r << 16) | (cf->floor.g << 8) | cf->floor.b;
}

static void	render_ceiling(t_game *game, t_ceiling_floor *cf)
{
	cf->y = 0;
	while (cf->y < WINDOW_HEIGHT / 2)
	{
		cf->x = 0;
		while (cf->x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, cf->x, cf->y, cf->ceiling_color);
			cf->x++;
		}
		cf->y++;
	}
}

static void	render_floor(t_game *game, t_ceiling_floor *cf)
{
	while (cf->y < WINDOW_HEIGHT)
	{
		cf->x = 0;
		while (cf->x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, cf->x, cf->y, cf->floor_color);
			cf->x++;
		}
		cf->y++;
	}
}

void	render_ceiling_floor(t_game *game)
{
	t_ceiling_floor	cf;

	init_ceiling_floor_colors(game, &cf);
	render_ceiling(game, &cf);
	render_floor(game, &cf);
} 