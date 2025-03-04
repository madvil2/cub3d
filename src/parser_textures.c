/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_textures.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
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