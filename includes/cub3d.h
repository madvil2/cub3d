#ifndef CUB3D_H
# define CUB3D_H

# include <mlx.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include "libft.h"

# define WINDOW_WIDTH 1280
# define WINDOW_HEIGHT 720
# define TITLE "cub3D"

# ifdef __APPLE__
#  define KEY_W 13
#  define KEY_A 0
#  define KEY_S 1
#  define KEY_D 2
#  define KEY_LEFT 123
#  define KEY_RIGHT 124
#  define KEY_ESC 53
#  define KEY_M 46
# else
#  define KEY_W 119
#  define KEY_A 97
#  define KEY_S 115
#  define KEY_D 100
#  define KEY_LEFT 65361
#  define KEY_RIGHT 65363
#  define KEY_ESC 65307
#  define KEY_M 109
# endif

# define ERR_ARGS "Error\nInvalid number of arguments"
# define ERR_FILE "Error\nCannot open file"
# define ERR_FORMAT "Error\nInvalid file format"
# define ERR_MAP "Error\nInvalid map configuration"
# define ERR_TEXTURE "Error\nInvalid texture configuration"
# define ERR_COLOR "Error\nInvalid color configuration"

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}	t_img;

typedef struct s_texture
{
	char	*path;
	void	*img;
	int		width;
	int		height;
}	t_texture;

typedef struct s_color
{
	int	r;
	int	g;
	int	b;
}	t_color;

typedef struct s_map
{
	char	**grid;
	int		width;
	int		height;
	int		player_x;
	int		player_y;
	char	player_dir;
}	t_map;

typedef struct s_config
{
	t_texture	north;
	t_texture	south;
	t_texture	west;
	t_texture	east;
	t_color		floor;
	t_color		ceiling;
	t_map		map;
}	t_config;

typedef struct s_player
{
	double	x;
	double	y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
}	t_player;

typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_img		img;
	t_player	player;
	t_config	config;
	int			last_mouse_x;
	int			mouse_control;
}	t_game;

typedef struct s_line_params
{
	int			tex_x;
	int			tex_y;
	char		*tex_data;
	int			tex_bpp;
	int			tex_line_size;
	int			tex_endian;
	double		step;
	double		tex_pos;
	t_texture	*tex;
}	t_line_params;

typedef struct s_ray_params
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
}	t_ray_params;

/* Game initialization and cleanup */
int		init_game(t_game *game);
void	cleanup_game(t_game *game);
int		handle_close(t_game *game);

/* Player movement */
int		check_collision(t_game *game, double new_x, double new_y);
void	handle_player_rotation(t_game *game, double rotation);
void	handle_player_movement(t_game *game, double dir_x, double dir_y);

/* Input handling */
int		handle_keypress(int keycode, t_game *game);
int		handle_mouse_move(int x, int y, t_game *game);
void	handle_mouse_toggle(t_game *game);

/* Game loop */
int		game_loop(t_game *game);

/* Parser functions */
int		parse_config(t_game *game, char *file_path);
int		parse_texture(char *line, t_texture *texture);
int		parse_color(char *line, t_color *color);
int		handle_map_line(t_game *game, char *line, int fd);
int		validate_map(t_map *map);
void	free_config(t_config *config);
void	debug_print_map(t_map *map);
void	debug_print_config(t_config *config);
int		is_empty_line(char *line);
int		free_split(char **split);
int		is_map_line(char *line);

/* Render functions */
void	draw_pixel(t_img *img, int x, int y, int color);
void	draw_rectangle(t_img *img, int x, int y, int size, int color);
void	flip_buffer_horizontally(t_img *img);
void	render_map(t_game *game);
void	render_frame(t_game *game);
void	render_3d_view(t_game *game);

void	load_texture(t_game *game, t_texture *texture);
void	load_textures(t_game *game);
void	draw_vertical_line(t_game *game, int x, int draw_start,
			int draw_end, int tex_x, t_texture *tex, double perp_wall_dist);

void	calculate_wall_drawing(double perp_wall_dist, int *line_height, 
			int *draw_start, int *draw_end);
void	calculate_wall_hit(t_game *game, double ray_dir_x, 
			double ray_dir_y, double perp_wall_dist, int side, 
			double *wall_x, t_texture **tex, int *tex_x);
void	render_ceiling_floor(t_game *game);
void	render_walls(t_game *game);

void	debug_ray_info(double ray_dir_x, double ray_dir_y, 
			double plane_x, double plane_y, double camera_x);
void	debug_wall_hit(int map_x, int map_y, double perp_wall_dist,
			int line_height, int draw_start, int draw_end,
			double wall_x, int tex_x, t_texture *tex, int side);
void	debug_texture_info(t_game *game);

void	cast_single_ray(t_game *game, int x, int *debug_count);

#endif