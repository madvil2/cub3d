/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_textures.c                                  :+:      :+:    :+:   */
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
