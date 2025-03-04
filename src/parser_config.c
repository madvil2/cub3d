/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	check_params_set(t_config *config)
{
	if (!config->north.path || !config->south.path
		|| !config->west.path || !config->east.path)
		return (0);
	if (config->floor.r < 0 || config->ceiling.r < 0)
		return (0);
	return (1);
}

static int	handle_config_directive(t_game *game, char *line, int i)
{
	if (!ft_strncmp(&line[i], "NO ", 3))
		return (parse_texture(&line[i + 3], &game->config.north));
	else if (!ft_strncmp(&line[i], "SO ", 3))
		return (parse_texture(&line[i + 3], &game->config.south));
	else if (!ft_strncmp(&line[i], "WE ", 3))
		return (parse_texture(&line[i + 3], &game->config.west));
	else if (!ft_strncmp(&line[i], "EA ", 3))
		return (parse_texture(&line[i + 3], &game->config.east));
	else if (!ft_strncmp(&line[i], "F ", 2))
		return (parse_color(&line[i + 2], &game->config.floor));
	else if (!ft_strncmp(&line[i], "C ", 2))
		return (parse_color(&line[i + 2], &game->config.ceiling));
	return (0);
}

static int	parse_config_line(t_game *game, char *line, int *params_set)
{
	int	i;

	if (is_empty_line(line))
		return (1);
	i = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] == '\0' || line[i] == '\n')
		return (1);
	if (!handle_config_directive(game, line, i))
		return (0);
	if (check_params_set(&game->config))
		*params_set = 1;
	return (1);
}

static int	handle_config_loop(t_game *game, int fd, int *params_set)
{
	char	*line;
	int		ret;

	ret = 1;
	while (ret && (line = get_next_line(fd)))
	{
		if (is_map_line(line))
		{
			*params_set = check_params_set(&game->config);
			if (!*params_set)
			{
				ft_putstr_fd("Error\nMap found before all parameters were set\n", 2);
				free(line);
				return (0);
			}
			ret = handle_map_line(game, line, fd);
			free(line);
			break;
		}
		else
			ret = parse_config_line(game, line, params_set);
		free(line);
	}
	return (ret);
}

int	parse_config(t_game *game, char *file_path)
{
	int		fd;
	int		params_set;
	int		ret;

	fd = open(file_path, O_RDONLY);
	if (fd < 0)
	{
		ft_putstr_fd(ERR_FILE, 2);
		ft_putstr_fd("\n", 2);
		return (0);
	}
	params_set = 0;
	ret = handle_config_loop(game, fd, &params_set);
	close(fd);
	return (ret && params_set);
} 