/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 21:48:31 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 22:12:40 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int	is_map_line(char *line)
{
	int	i;
	int	result;

	i = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && (
			(line[i] == 'N' && line[i + 1] == 'O' && line[i + 2] == ' ')
			|| (line[i] == 'S' && line[i + 1] == 'O' && line[i + 2] == ' ')
			|| (line[i] == 'W' && line[i + 1] == 'E' && line[i + 2] == ' ')
			|| (line[i] == 'E' && line[i + 1] == 'A' && line[i + 2] == ' ')
			|| (line[i] == 'F' && line[i + 1] == ' ')
			|| (line[i] == 'C' && line[i + 1] == ' ')))
	{
		result = 0;
	}
	else
	{
		result = (line[i] && (line[i] == '1' || line[i] == '0'
					|| line[i] == 'N' || line[i] == 'S'
					|| line[i] == 'E' || line[i] == 'W'));
	}
	return (result);
}

static int	process_map(t_game *game, char **map_lines)
{
	t_map	*map;

	if (!map_lines)
		return (0);
	map = &game->config.map;
	get_map_dimensions(map, map_lines);
	if (!alloc_map_grid(map))
		return (0);
	copy_map_data(map, map_lines);
	free_map_lines(map_lines);
	return (validate_map(map));
}

int	handle_map_line(t_game *game, char *line, int fd)
{
	char	**map_lines;
	int		ret;

	map_lines = collect_map_lines(line, fd);
	ret = process_map(game, map_lines);
	if (ret)
		debug_print_map(&game->config.map);
	return (ret);
}
