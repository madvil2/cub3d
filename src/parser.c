/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by student           #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	debug_print_map(t_map *map)
{
	(void)map;
}

static void	debug_print_config(t_config *config)
{
	(void)config;
}

static int	is_empty_line(char *line)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
			return (0);
		i++;
	}
	return (1);
}

static int	free_split(char **split)
{
	int	i;

	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
	return (0);
}

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

static int	is_map_line(char *line)
{
	int	i;
	int	result;

	i = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	
	if (line[i] && (
		(line[i] == 'N' && line[i+1] && line[i+1] == 'O' && line[i+2] && line[i+2] == ' ') ||
		(line[i] == 'S' && line[i+1] && line[i+1] == 'O' && line[i+2] && line[i+2] == ' ') ||
		(line[i] == 'W' && line[i+1] && line[i+1] == 'E' && line[i+2] && line[i+2] == ' ') ||
		(line[i] == 'E' && line[i+1] && line[i+1] == 'A' && line[i+2] && line[i+2] == ' ') ||
		(line[i] == 'F' && line[i+1] && line[i+1] == ' ') ||
		(line[i] == 'C' && line[i+1] && line[i+1] == ' ')))
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

static int	is_valid_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S'
		|| c == 'E' || c == 'W' || c == ' ' || c == '\n');
}

static int	check_wall_surroundings(t_map *map, int i, int j)
{
	if (i == 0 || i == map->height - 1 || j == 0 || j == map->width - 1
		|| map->grid[i - 1][j] == ' ' || map->grid[i + 1][j] == ' '
		|| map->grid[i][j - 1] == ' ' || map->grid[i][j + 1] == ' ')
		return (0);
	return (1);
}

static int	validate_map_walls(t_map *map)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width && map->grid[i][j])
		{
			if (map->grid[i][j] == '0' || map->grid[i][j] == 'N'
				|| map->grid[i][j] == 'S' || map->grid[i][j] == 'E'
				|| map->grid[i][j] == 'W')
			{
				if (!check_wall_surroundings(map, i, j))
					return (0);
			}
			j++;
		}
		i++;
	}
	return (1);
}

static int	validate_map_chars(t_map *map, int *player_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		j = 0;
		while (j < map->width && map->grid[i][j])
		{
			if (!is_valid_map_char(map->grid[i][j]))
				return (0);
			if (map->grid[i][j] == 'N' || map->grid[i][j] == 'S'
				|| map->grid[i][j] == 'E' || map->grid[i][j] == 'W')
			{
				(*player_count)++;
				map->player_x = j;
				map->player_y = i;
				map->player_dir = map->grid[i][j];
			}
			j++;
		}
		i++;
	}
	return (1);
}

static int	validate_map(t_map *map)
{
	int	player_count;

	player_count = 0;
	if (!validate_map_chars(map, &player_count))
		return (0);
	if (!validate_map_walls(map))
		return (0);
	return (player_count == 1);
}

static int	handle_empty_line_in_map(char **lines, int i, char *line)
{
	free(line);
	while (--i >= 0)
		free(lines[i]);
	free(lines);
	return (-1);
}

static int	process_line(char **lines, int *i, char *line)
{
	if (is_empty_line(line))
	{
		if (*i > 0)
		{
			if (handle_empty_line_in_map(lines, *i, line) == -1)
				return (0);
		}
		else
			free(line);
	}
	else
	{
		lines[*i] = ft_strdup(line);
		(*i)++;
		free(line);
	}
	return (1);
}

static char	**collect_map_lines(char *first_line, int fd)
{
	char	**lines;
	char	*line;
	int		i;

	lines = malloc(sizeof(char *) * 1000);
	if (!lines)
		return (NULL);
	lines[0] = ft_strdup(first_line);
	i = 1;
	while ((line = get_next_line(fd)) != NULL)
	{
		if (!process_line(lines, &i, line))
			return (NULL);
	}
	lines[i] = NULL;
	return (lines);
}

static int	alloc_map_grid(t_map *map)
{
	int	i;

	map->grid = malloc(sizeof(char *) * (map->height + 1));
	if (!map->grid)
		return (0);
	i = 0;
	while (i < map->height)
	{
		map->grid[i] = malloc(sizeof(char) * (map->width + 1));
		if (!map->grid[i])
		{
			while (--i >= 0)
				free(map->grid[i]);
			free(map->grid);
			return (0);
		}
		i++;
	}
	return (1);
}

static void	copy_map_data(t_map *map, char **map_lines)
{
	int	i;
	int	j;

	i = 0;
	while (i < map->height)
	{
		ft_memset(map->grid[i], ' ', map->width);
		j = 0;
		while (map_lines[i][j] && map_lines[i][j] != '\n')
		{
			map->grid[i][j] = map_lines[i][j];
			j++;
		}
		map->grid[i][map->width] = '\0';
		i++;
	}
	map->grid[map->height] = NULL;
}

static void	free_map_lines(char **map_lines)
{
	int	i;

	i = 0;
	while (map_lines[i])
	{
		free(map_lines[i]);
		i++;
	}
	free(map_lines);
}

static int	get_map_dimensions(t_map *map, char **map_lines)
{
	int	i;
	int	j;
	int	max_width;

	map->height = 0;
	while (map_lines[map->height])
		map->height++;
	max_width = 0;
	i = 0;
	while (map_lines[i])
	{
		j = ft_strlen(map_lines[i]);
		if (j > max_width)
			max_width = j;
		i++;
	}
	map->width = max_width;
	return (1);
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

static int	check_params_set(t_config *config)
{
	int	north;
	int	south;
	int	west;
	int	east;
	int	floor;
	int	ceiling;

	north = config->north.path != NULL;
	south = config->south.path != NULL;
	west = config->west.path != NULL;
	east = config->east.path != NULL;
	floor = config->floor.r != -1;
	ceiling = config->ceiling.r != -1;
	return (north && south && west && east && floor && ceiling);
}

static int	handle_map_line(t_game *game, char *line, int fd)
{
	char	**map_lines;
	int		ret;

	map_lines = collect_map_lines(line, fd);
	ret = process_map(game, map_lines);
	if (ret)
		debug_print_map(&game->config.map);
	return (ret);
}

static int	handle_config_directive(t_game *game, char *line, int i)
{
	if (ft_strncmp(&line[i], "NO ", 3) == 0)
		return (parse_texture(&line[i + 3], &game->config.north));
	else if (ft_strncmp(&line[i], "SO ", 3) == 0)
		return (parse_texture(&line[i + 3], &game->config.south));
	else if (ft_strncmp(&line[i], "WE ", 3) == 0)
		return (parse_texture(&line[i + 3], &game->config.west));
	else if (ft_strncmp(&line[i], "EA ", 3) == 0)
		return (parse_texture(&line[i + 3], &game->config.east));
	else if (ft_strncmp(&line[i], "F ", 2) == 0)
		return (parse_color(&line[i + 2], &game->config.floor));
	else if (ft_strncmp(&line[i], "C ", 2) == 0)
		return (parse_color(&line[i + 2], &game->config.ceiling));
	return (1);
}

static int	parse_config_line(t_game *game, char *line, int *params_set)
{
	int	i;
	int	ret;

	i = 0;
	ret = 1;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (is_empty_line(&line[i]))
		return (1);
	ret = handle_config_directive(game, line, i);
	*params_set = check_params_set(&game->config);
	if (*params_set)
		debug_print_config(&game->config);
	return (ret);
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

void	free_config(t_config *config)
{
	int	i;

	if (config->north.path)
		free(config->north.path);
	if (config->south.path)
		free(config->south.path);
	if (config->west.path)
		free(config->west.path);
	if (config->east.path)
		free(config->east.path);
	if (config->map.grid)
	{
		i = 0;
		while (i < config->map.height)
			free(config->map.grid[i++]);
		free(config->map.grid);
	}
} 