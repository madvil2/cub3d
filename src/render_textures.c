/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_textures.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:32:24 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 22:15:05 by kokaimov         ###   ########.fr       */
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
