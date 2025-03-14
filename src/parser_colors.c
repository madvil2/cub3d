/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_colors.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:19:07 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 21:58:39 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	parse_rgb_values(char **split, t_color *color)
{
	color->r = ft_atoi(split[0]);
	color->g = ft_atoi(split[1]);
	color->b = ft_atoi(split[2]);
	if (color->r < 0 || color->r > 255 || 
		color->g < 0 || color->g > 255 || 
		color->b < 0 || color->b > 255)
		return (0);
	return (1);
}

static int	parse_rgb(char *rgb_str, t_color *color)
{
	char	**split;
	int		count;
	int		result;

	split = ft_split(rgb_str, ',');
	if (!split)
		return (0);
	count = 0;
	while (split[count])
		count++;
	if (count != 3)
		return (free_split(split));
	result = parse_rgb_values(split, color);
	free_split(split);
	return (result);
}

int	parse_color(char *line, t_color *color)
{
	int		i;
	int		j;
	char	*trimmed;
	int		ret;

	i = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	j = ft_strlen(line);
	while (j > i && (line[j - 1] == ' ' || line[j - 1] == '\t'
			|| line[j - 1] == '\n'))
		j--;
	trimmed = ft_substr(line, i, j - i);
	if (!trimmed)
		return (0);
	ret = parse_rgb(trimmed, color);
	free(trimmed);
	return (ret);
}
