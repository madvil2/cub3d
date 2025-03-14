/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_textures.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:19:07 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 21:53:18 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int	parse_texture(char *line, t_texture *texture)
{
	int	i;
	int	len;

	i = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	texture->path = ft_strdup(&line[i]);
	if (!texture->path)
		return (0);
	len = ft_strlen(texture->path) - 1;
	if (texture->path[len] == '\n')
		texture->path[len] = '\0';
	return (1);
}
