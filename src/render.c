/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

# define MAP_SCALE 16
# define PLAYER_SIZE 4

void		draw_pixel(t_img *img, int x, int y, int color);
static void	draw_rectangle(t_img *img, int x, int y, int size, int color);
static void	draw_player(t_game *game, int map_x, int map_y);
static void	flip_minimap(t_game *game, int map_x, int map_y, int w, int h);
void		render_map(t_game *game);
void		load_texture(t_game *game, t_texture *texture);
void		load_textures(t_game *game);
void		draw_vertical_line(t_game *game, int x, int draw_start,
			int draw_end, int tex_x, t_texture *tex, double perp_wall_dist);
static void	calculate_texture_params(int *tex_y, double *tex_pos,
			double *step, t_texture *tex, int line_height,
			int draw_start, double perp_wall_dist);
static void	calculate_wall_hit(t_game *game, double ray_dir_x,
			double ray_dir_y, double perp_wall_dist, int side,
			double *wall_x, t_texture **tex, int *tex_x);
static void	calculate_wall_drawing(double perp_wall_dist, int *line_height,
			int *draw_start, int *draw_end);
static void	calculate_ray_step(t_game *game, double ray_dir_x,
			double ray_dir_y, int *step_x, int *step_y,
			double *side_dist_x, double *side_dist_y);
static void	perform_dda(t_game *game, int *map_x, int *map_y,
			int step_x, int step_y, double *side_dist_x,
			double *side_dist_y, double delta_dist_x,
			double delta_dist_y, int *side);
static void	render_ceiling_floor(t_game *game);
static void	render_walls(t_game *game);
void		render_3d_view(t_game *game);
static void	flip_buffer_horizontally(t_img *img);
static void	debug_ray_info(double ray_dir_x, double ray_dir_y,
			double plane_x, double plane_y, double camera_x);
static void	debug_wall_hit(int map_x, int map_y, double perp_wall_dist,
			int line_height, int draw_start, int draw_end,
			double wall_x, int tex_x, t_texture *tex, int side);
static void	debug_texture_info(t_game *game);

void	draw_pixel(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
	{
		dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
		*(unsigned int *)dst = color;
	}
}

static void	draw_rectangle(t_img *img, int x, int y, int size, int color)
{
	int	i;
	int	j;

	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			draw_pixel(img, x + i, y + j, color);
			j++;
		}
		i++;
	}
}

static void	draw_player_position(t_game *game, int center_x, int center_y)
{
	int	i;
	int	j;

	i = -PLAYER_SIZE / 2;
	while (i <= PLAYER_SIZE / 2)
	{
		j = -PLAYER_SIZE / 2;
		while (j <= PLAYER_SIZE / 2)
		{
			draw_pixel(&game->img, center_x + i, center_y + j, 0xFF0000);
			j++;
		}
		i++;
	}
}

static void	draw_player_direction(t_game *game, int center_x, int center_y)
{
	int		dir_x;
	int		dir_y;
	double	dx;
	double	dy;
	double	steps;
	double	x_inc;
	double	y_inc;
	double	x;
	double	y;
	int		i;

	dir_x = center_x + (int)(game->player.dir_x * MAP_SCALE);
	dir_y = center_y + (int)(game->player.dir_y * MAP_SCALE);
	dx = dir_x - center_x;
	dy = dir_y - center_y;
	steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
	x_inc = dx / steps;
	y_inc = dy / steps;
	x = center_x;
	y = center_y;
	i = 0;
	while (i <= steps)
	{
		draw_pixel(&game->img, (int)x, (int)y, 0xFF0000);
		x += x_inc;
		y += y_inc;
		i++;
	}
}

static void	draw_player(t_game *game, int map_x, int map_y)
{
	int	center_x;
	int	center_y;

	center_x = map_x + (int)(game->player.x * MAP_SCALE);
	center_y = map_y + (int)(game->player.y * MAP_SCALE);
	draw_player_position(game, center_x, center_y);
	draw_player_direction(game, center_x, center_y);
}

static void	swap_pixels(t_game *game, int x1, int y1, int x2, int y2)
{
	unsigned int	left_color;
	unsigned int	right_color;
	char			*left_addr;
	char			*right_addr;
	int				bytes_per_pixel;

	bytes_per_pixel = game->img.bits_per_pixel / 8;
	left_addr = game->img.addr + (y1 * game->img.line_length + 
		x1 * bytes_per_pixel);
	right_addr = game->img.addr + (y2 * game->img.line_length + 
		x2 * bytes_per_pixel);
	
	left_color = *(unsigned int *)left_addr;
	right_color = *(unsigned int *)right_addr;
	
	draw_pixel(&game->img, x1, y1, right_color);
	draw_pixel(&game->img, x2, y2, left_color);
}

static void	flip_minimap(t_game *game, int map_x, int map_y, int w, int h)
{
	int	x;
	int	y;
	int	half_width;

	half_width = w / 2;
	y = 0;
	while (y < h)
	{
		x = 0;
		while (x < half_width)
		{
			swap_pixels(game, x + map_x, y + map_y, 
				w - 1 - x + map_x, y + map_y);
			x++;
		}
		y++;
	}
}

static int	get_cell_color(t_game *game, int x, int y)
{
	if (game->config.map.grid[y][x] == '1')
		return (0xFFFFFF);
	else if (game->config.map.grid[y][x] == '0' || 
			game->config.map.grid[y][x] == 'N' ||
			game->config.map.grid[y][x] == 'S' ||
			game->config.map.grid[y][x] == 'E' ||
			game->config.map.grid[y][x] == 'W')
		return (0x404040);
	else
		return (0x000000);
}

void	render_map(t_game *game)
{
	int	x;
	int	y;
	int	map_x;
	int	map_y;
	int	map_width;
	int	map_height;

	map_x = 10;
	map_y = 10;
	map_width = game->config.map.width * MAP_SCALE;
	map_height = game->config.map.height * MAP_SCALE;
	y = 0;
	while (y < game->config.map.height)
	{
		x = 0;
		while (x < game->config.map.width)
		{
			draw_rectangle(&game->img, map_x + x * MAP_SCALE, 
				map_y + y * MAP_SCALE, MAP_SCALE - 1, 
				get_cell_color(game, x, y));
			x++;
		}
		y++;
	}
	draw_player(game, map_x, map_y);
	flip_minimap(game, map_x, map_y, map_width, map_height);
}

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

static void	calculate_texture_close(double perp_wall_dist, t_texture *tex, 
	double *step, double *tex_pos, int line_height, int draw_start)
{
	double	zoom_factor;
	double	visible_portion;
	double	player_height_ratio;
	double	center_of_tex;
	double	window_offset_ratio;

	zoom_factor = 1.0 / perp_wall_dist;
	if (zoom_factor > 20.0)
		zoom_factor = 20.0;
	visible_portion = 1.0 / zoom_factor;
	player_height_ratio = 0.5;
	center_of_tex = tex->height * player_height_ratio;
	*tex_pos = center_of_tex - ((visible_portion * tex->height) 
		* player_height_ratio);
	window_offset_ratio = (double)draw_start / WINDOW_HEIGHT;
	*tex_pos += window_offset_ratio * visible_portion * tex->height;
	*step = (visible_portion * tex->height) / line_height;
}

static void	calculate_texture_params(int *tex_y, double *tex_pos, 
	double *step, t_texture *tex, int line_height, 
	int draw_start, double perp_wall_dist)
{
	if (perp_wall_dist < 1.0)
		calculate_texture_close(perp_wall_dist, tex, step, tex_pos, 
			line_height, draw_start);
	else
	{
		*step = (double)tex->height / line_height;
		*tex_pos = 0;
	}
	*tex_y = (int)*tex_pos;
	if (*tex_y >= tex->height)
		*tex_y = tex->height - 1;
	if (*tex_y < 0)
		*tex_y = 0;
}

static void	init_vertical_line_params(int tex_x, t_texture *tex,
	double perp_wall_dist, int draw_start, int line_height,
	t_line_params *params)
{
	if (tex_x >= tex->width)
		tex_x = tex->width - 1;
	if (tex_x < 0)
		tex_x = 0;
	params->tex_x = tex_x;
	params->tex = tex;
	params->tex_data = mlx_get_data_addr(tex->img, &params->tex_bpp,
		&params->tex_line_size, &params->tex_endian);
	calculate_texture_params(&params->tex_y, &params->tex_pos, &params->step,
		tex, line_height, draw_start, perp_wall_dist);
}

void	draw_vertical_line(t_game *game, int x, int draw_start, 
	int draw_end, int tex_x, t_texture *tex, double perp_wall_dist)
{
	int				y;
	int				color;
	int				line_height;
	t_line_params	params;

	line_height = draw_end - draw_start;
	init_vertical_line_params(tex_x, tex, perp_wall_dist, 
		draw_start, line_height, &params);
	y = draw_start;
	while (y < draw_end)
	{
		params.tex_y = (int)params.tex_pos;
		if (params.tex_y >= params.tex->height)
			params.tex_y = params.tex->height - 1;
		if (params.tex_y < 0)
			params.tex_y = 0;
		color = *(unsigned int *)(params.tex_data + (params.tex_y 
			* params.tex_line_size + params.tex_x * (params.tex_bpp / 8)));
		draw_pixel(&game->img, x, y, color);
		params.tex_pos += params.step;
		y++;
	}
}

static void	calculate_ray_step(t_game *game, double ray_dir_x, 
	double ray_dir_y, int *step_x, int *step_y, 
	double *side_dist_x, double *side_dist_y)
{
	int	map_x;
	int	map_y;

	map_x = (int)game->player.x;
	map_y = (int)game->player.y;
	if (ray_dir_x < 0)
	{
		*step_x = -1;
		*side_dist_x = (game->player.x - map_x) * fabs(1 / ray_dir_x);
	}
	else
	{
		*step_x = 1;
		*side_dist_x = (map_x + 1.0 - game->player.x) * fabs(1 / ray_dir_x);
	}
	if (ray_dir_y < 0)
	{
		*step_y = -1;
		*side_dist_y = (game->player.y - map_y) * fabs(1 / ray_dir_y);
	}
	else
	{
		*step_y = 1;
		*side_dist_y = (map_y + 1.0 - game->player.y) * fabs(1 / ray_dir_y);
	}
}

static void	perform_dda(t_game *game, int *map_x, int *map_y, 
	int step_x, int step_y, double *side_dist_x, 
	double *side_dist_y, double delta_dist_x, 
	double delta_dist_y, int *side)
{
	int	hit;

	hit = 0;
	*map_x = (int)game->player.x;
	*map_y = (int)game->player.y;
	while (hit == 0)
	{
		if (*side_dist_x < *side_dist_y)
		{
			*side_dist_x += delta_dist_x;
			*map_x += step_x;
			*side = 0;
		}
		else
		{
			*side_dist_y += delta_dist_y;
			*map_y += step_y;
			*side = 1;
		}
		if (game->config.map.grid[*map_y][*map_x] == '1')
			hit = 1;
	}
}

static void	calculate_wall_drawing(double perp_wall_dist, int *line_height, 
	int *draw_start, int *draw_end)
{
	if (perp_wall_dist <= 0.1)
		perp_wall_dist = 0.1;
	*line_height = (int)(WINDOW_HEIGHT / perp_wall_dist);
	*draw_start = -(*line_height) / 2 + WINDOW_HEIGHT / 2;
	if (*draw_start < 0)
		*draw_start = 0;
	*draw_end = (*line_height) / 2 + WINDOW_HEIGHT / 2;
	if (*draw_end >= WINDOW_HEIGHT)
		*draw_end = WINDOW_HEIGHT - 1;
}

static void	set_texture_by_direction(double ray_dir_x, double ray_dir_y,
	int side, t_texture **tex, t_game *game)
{
	if (side == 0)
	{
		if (ray_dir_x > 0)
			*tex = &game->config.east;
		else
			*tex = &game->config.west;
	}
	else
	{
		if (ray_dir_y > 0)
			*tex = &game->config.south;
		else
			*tex = &game->config.north;
	}
}

static void	calculate_wall_hit(t_game *game, double ray_dir_x, 
	double ray_dir_y, double perp_wall_dist, int side, 
	double *wall_x, t_texture **tex, int *tex_x)
{
	if (side == 0)
		*wall_x = game->player.y + perp_wall_dist * ray_dir_y;
	else
		*wall_x = game->player.x + perp_wall_dist * ray_dir_x;
	
	set_texture_by_direction(ray_dir_x, ray_dir_y, side, tex, game);
	
	*wall_x -= floor(*wall_x);
	*tex_x = (int)(*wall_x * (*tex)->width);
	if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0))
		*tex_x = (*tex)->width - *tex_x - 1;
}

static void	debug_ray_info(double ray_dir_x, double ray_dir_y, 
	double plane_x, double plane_y, double camera_x)
{
	(void)ray_dir_x;
	(void)ray_dir_y;
	(void)plane_x;
	(void)plane_y;
	(void)camera_x;
}

static void	debug_wall_hit(int map_x, int map_y, double perp_wall_dist,
	int line_height, int draw_start, int draw_end,
	double wall_x, int tex_x, t_texture *tex, int side)
{
	(void)map_x;
	(void)map_y;
	(void)perp_wall_dist;
	(void)line_height;
	(void)draw_start;
	(void)draw_end;
	(void)wall_x;
	(void)tex_x;
	(void)tex;
	(void)side;
}

static void	render_ceiling_floor(t_game *game)
{
	int	x;
	int	y;
	int	ceiling_color;
	int	floor_color;
	t_color ceiling;
	t_color floor;

	ceiling = game->config.ceiling;
	floor = game->config.floor;
	ceiling_color = (ceiling.r << 16) | (ceiling.g << 8) | ceiling.b;
	floor_color = (floor.r << 16) | (floor.g << 8) | floor.b;
	
	y = 0;
	while (y < WINDOW_HEIGHT / 2)
	{
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, x, y, ceiling_color);
			x++;
		}
		y++;
	}
	while (y < WINDOW_HEIGHT)
	{
		x = 0;
		while (x < WINDOW_WIDTH)
		{
			draw_pixel(&game->img, x, y, floor_color);
			x++;
		}
		y++;
	}
}

static void	prepare_ray_data(t_game *game, int x, double *ray_dir_x,
	double *ray_dir_y, double *delta_dist_x, double *delta_dist_y)
{
	double	camera_x;

	camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
	*ray_dir_x = game->player.dir_x + game->player.plane_x * camera_x;
	*ray_dir_y = game->player.dir_y + game->player.plane_y * camera_x;
	*delta_dist_x = fabs(1 / *ray_dir_x);
	*delta_dist_y = fabs(1 / *ray_dir_y);
}

static void	cast_single_ray(t_game *game, int x, int *debug_count)
{
	double		ray_dir_x;
	double		ray_dir_y;
	int			map_x;
	int			map_y;
	double		side_dist_x;
	double		side_dist_y;
	double		delta_dist_x;
	double		delta_dist_y;
	double		perp_wall_dist;
	int			step_x;
	int			step_y;
	int			side;
	int			line_height;
	int			draw_start;
	int			draw_end;
	t_texture	*tex;
	int			tex_x;
	double		wall_x;

	prepare_ray_data(game, x, &ray_dir_x, &ray_dir_y, &delta_dist_x, &delta_dist_y);
	if ((*debug_count)++ % 60 == 0 && x == WINDOW_WIDTH / 2)
		debug_ray_info(ray_dir_x, ray_dir_y, game->player.plane_x, 
			game->player.plane_y, 2 * x / (double)WINDOW_WIDTH - 1);
	calculate_ray_step(game, ray_dir_x, ray_dir_y, &step_x, &step_y, 
		&side_dist_x, &side_dist_y);
	perform_dda(game, &map_x, &map_y, step_x, step_y, &side_dist_x, 
		&side_dist_y, delta_dist_x, delta_dist_y, &side);
	if (side == 0)
		perp_wall_dist = (side_dist_x - delta_dist_x);
	else
		perp_wall_dist = (side_dist_y - delta_dist_y);
	calculate_wall_drawing(perp_wall_dist, &line_height, &draw_start, &draw_end);
	calculate_wall_hit(game, ray_dir_x, ray_dir_y, perp_wall_dist, 
		side, &wall_x, &tex, &tex_x);
	if (*debug_count % 60 == 1 && x == WINDOW_WIDTH / 2)
		debug_wall_hit(map_x, map_y, perp_wall_dist, line_height, 
			draw_start, draw_end, wall_x, tex_x, tex, side);
	draw_vertical_line(game, x, draw_start, draw_end, tex_x, tex, perp_wall_dist);
}

static void	render_walls(t_game *game)
{
	int			x;
	static int	debug_count = 0;

	x = 0;
	while (x < WINDOW_WIDTH)
	{
		cast_single_ray(game, x, &debug_count);
		x++;
	}
}

void	render_3d_view(t_game *game)
{
	render_ceiling_floor(game);
	render_walls(game);
}

static void	flip_buffer_horizontally(t_img *img)
{
	int			x;
	int			y;
	unsigned int	temp;
	char		*buffer;
	int			bytes_per_pixel;
	char		*left_pixel;
	char		*right_pixel;

	buffer = img->addr;
	bytes_per_pixel = img->bits_per_pixel / 8;
	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		x = 0;
		while (x < WINDOW_WIDTH / 2)
		{
			left_pixel = buffer + (y * img->line_length + x * bytes_per_pixel);
			right_pixel = buffer + (y * img->line_length + 
				(WINDOW_WIDTH - 1 - x) * bytes_per_pixel);
			temp = *(unsigned int *)left_pixel;
			*(unsigned int *)left_pixel = *(unsigned int *)right_pixel;
			*(unsigned int *)right_pixel = temp;
			x++;
		}
		y++;
	}
}

static void	debug_texture_info(t_game *game)
{
	(void)game;
}

void	render_frame(t_game *game)
{
	static int	frame_count = 0;
	int			buffer_size;

	if (frame_count++ % 60 == 0)
		debug_texture_info(game);
	
	buffer_size = WINDOW_WIDTH * WINDOW_HEIGHT * 
		(game->img.bits_per_pixel / 8);
	ft_memset(game->img.addr, 0, buffer_size);
	
	render_3d_view(game);
	render_map(game);
	flip_buffer_horizontally(&game->img);
	mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
} 