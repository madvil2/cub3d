/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:43:20 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/04 18:58:47 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define MOVE_SPEED 0.05
#define ROT_SPEED 0.05
#define WALL_OFFSET 0.2
#define MOUSE_SENSITIVITY 0.002

int init_game(t_game *game)
{
    game->mlx = mlx_init();
    if (!game->mlx)
        return (0);
    
    game->win = mlx_new_window(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
    if (!game->win)
        return (0);
    
    game->img.img = mlx_new_image(game->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!game->img.img)
        return (0);
    
    game->img.addr = mlx_get_data_addr(game->img.img, &game->img.bits_per_pixel,
                                      &game->img.line_length, &game->img.endian);
    
    // Initialize mouse control
    game->mouse_control = 0;
    game->last_mouse_x = WINDOW_WIDTH / 2;
    
    // Load textures
    load_textures(game);
    
    // Initialize player position and direction based on map configuration
    game->player.x = game->config.map.player_x + 0.5;
    game->player.y = game->config.map.player_y + 0.5;
    
    // Set initial direction based on player spawn orientation
    if (game->config.map.player_dir == 'N')
    {
        game->player.dir_x = 0;
        game->player.dir_y = -1;
    }
    else if (game->config.map.player_dir == 'S')
    {
        game->player.dir_x = 0;
        game->player.dir_y = 1;
    }
    else if (game->config.map.player_dir == 'E')
    {
        game->player.dir_x = 1;
        game->player.dir_y = 0;
    }
    else // 'W'
    {
        game->player.dir_x = -1;
        game->player.dir_y = 0;
    }
    
    // Set the camera plane perpendicular to direction
    game->player.plane_x = game->player.dir_y * 0.66;
    game->player.plane_y = -game->player.dir_x * 0.66;
    
    return (1);
}

void cleanup_game(t_game *game)
{
	if (game->config.north.img)
		mlx_destroy_image(game->mlx, game->config.north.img);
	if (game->config.south.img)
		mlx_destroy_image(game->mlx, game->config.south.img);
	if (game->config.west.img)
		mlx_destroy_image(game->mlx, game->config.west.img);
	if (game->config.east.img)
		mlx_destroy_image(game->mlx, game->config.east.img);
    if (game->img.img)
        mlx_destroy_image(game->mlx, game->img.img);
    if (game->win)
        mlx_destroy_window(game->mlx, game->win);
    if (game->mlx)
        mlx_destroy_display(game->mlx);
    free_config(&game->config);
	if (game->mlx)
		free(game->mlx);
    exit(0);
}

int handle_close(t_game *game)
{
    cleanup_game(game);
    return (0);
}

static int check_collision(t_game *game, double new_x, double new_y)
{
    // Check if the new position would be inside a wall
    if (game->config.map.grid[(int)new_y][(int)new_x] == '1')
        return (1);
    
    // Check with offset for smoother collisions
    if (game->config.map.grid[(int)(new_y + WALL_OFFSET)][(int)(new_x + WALL_OFFSET)] == '1' ||
        game->config.map.grid[(int)(new_y + WALL_OFFSET)][(int)(new_x - WALL_OFFSET)] == '1' ||
        game->config.map.grid[(int)(new_y - WALL_OFFSET)][(int)(new_x + WALL_OFFSET)] == '1' ||
        game->config.map.grid[(int)(new_y - WALL_OFFSET)][(int)(new_x - WALL_OFFSET)] == '1')
        return (1);
    return (0);
}

int handle_mouse_move(int x, int y, t_game *game)
{
    double oldDirX;
    double oldPlaneX;
    double rotation;
    static int last_x = -1;
    
    (void)y; // Unused parameter
    
    if (!game->mouse_control)
        return (0);
        
    if (last_x == -1)
    {
        last_x = x;
        return (0);
    }
    
    // Calculate rotation based on mouse movement
    rotation = (x - last_x) * MOUSE_SENSITIVITY;
    last_x = x;
    
    // Rotate direction vector
    oldDirX = game->player.dir_x;
    game->player.dir_x = game->player.dir_x * cos(rotation) - game->player.dir_y * sin(rotation);
    game->player.dir_y = oldDirX * sin(rotation) + game->player.dir_y * cos(rotation);
    
    // Rotate camera plane
    oldPlaneX = game->player.plane_x;
    game->player.plane_x = game->player.plane_x * cos(rotation) - game->player.plane_y * sin(rotation);
    game->player.plane_y = oldPlaneX * sin(rotation) + game->player.plane_y * cos(rotation);
    
    // Reset mouse position if it gets too close to the screen edges
    if (x < 100 || x > WINDOW_WIDTH - 100)
    {
		mlx_mouse_move(game->mlx, game->win, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		last_x = WINDOW_WIDTH / 2;
    }
    
    return (0);
}

int handle_keypress(int keycode, t_game *game)
{
    double oldDirX;
    double oldPlaneX;
    double new_x;
    double new_y;

    if (keycode == ESC) // ESC key
        cleanup_game(game);
    else if (keycode == M) // M key to toggle mouse control
    {
        game->mouse_control = !game->mouse_control;
        if (game->mouse_control)
            mlx_mouse_hide(game->mlx, game->win);
        else
            mlx_mouse_show(game->mlx, game->win);
    }
    else if (keycode == W) // W key
    {
        new_x = game->player.x + game->player.dir_x * MOVE_SPEED;
        new_y = game->player.y + game->player.dir_y * MOVE_SPEED;
        if (!check_collision(game, new_x, new_y))
        {
            game->player.x = new_x;
            game->player.y = new_y;
        }
    }
    else if (keycode == S) // S key
    {
        new_x = game->player.x - game->player.dir_x * MOVE_SPEED;
        new_y = game->player.y - game->player.dir_y * MOVE_SPEED;
        if (!check_collision(game, new_x, new_y))
        {
            game->player.x = new_x;
            game->player.y = new_y;
        }
    }
    else if (keycode == D) // D key
    {
        new_x = game->player.x - game->player.plane_x * MOVE_SPEED;
        new_y = game->player.y - game->player.plane_y * MOVE_SPEED;
        if (!check_collision(game, new_x, new_y))
        {
            game->player.x = new_x;
            game->player.y = new_y;
        }
    }
    else if (keycode == A) // A key
    {
        new_x = game->player.x + game->player.plane_x * MOVE_SPEED;
        new_y = game->player.y + game->player.plane_y * MOVE_SPEED;
        if (!check_collision(game, new_x, new_y))
        {
            game->player.x = new_x;
            game->player.y = new_y;
        }
    }
    else if (keycode == RIGHTARR) // Right arrow
    {
        oldDirX = game->player.dir_x;
        game->player.dir_x = game->player.dir_x * cos(ROT_SPEED) - game->player.dir_y * sin(ROT_SPEED);
        game->player.dir_y = oldDirX * sin(ROT_SPEED) + game->player.dir_y * cos(ROT_SPEED);
        oldPlaneX = game->player.plane_x;
        game->player.plane_x = game->player.plane_x * cos(ROT_SPEED) - game->player.plane_y * sin(ROT_SPEED);
        game->player.plane_y = oldPlaneX * sin(ROT_SPEED) + game->player.plane_y * cos(ROT_SPEED);
    }
    else if (keycode == LEFTARR) // Left arrow
    {
        oldDirX = game->player.dir_x;
        game->player.dir_x = game->player.dir_x * cos(-ROT_SPEED) - game->player.dir_y * sin(-ROT_SPEED);
        game->player.dir_y = oldDirX * sin(-ROT_SPEED) + game->player.dir_y * cos(-ROT_SPEED);
        oldPlaneX = game->player.plane_x;
        game->player.plane_x = game->player.plane_x * cos(-ROT_SPEED) - game->player.plane_y * sin(-ROT_SPEED);
        game->player.plane_y = oldPlaneX * sin(-ROT_SPEED) + game->player.plane_y * cos(-ROT_SPEED);
    }
    return (0);
}

int game_loop(t_game *game)
{
    render_frame(game);
    return (0);
}

int main(int argc, char **argv)
{
    t_game  game;

    if (argc != 2)
    {
        printf("%s\n", ERR_ARGS);
        return (1);
    }

    // Initialize game config with default values
    ft_memset(&game.config, 0, sizeof(t_config));
    game.config.floor.r = -1;
    game.config.ceiling.r = -1;

    // Parse configuration file
    if (!parse_config(&game, argv[1]))
    {
        printf("%s\n", ERR_FORMAT);
        return (1);
    }

    // Initialize game
    if (!init_game(&game))
    {
        printf("Error\nFailed to initialize game\n");
        return (1);
    }

    // Set up event hooks
    mlx_hook(game.win, 2, 1L<<0, handle_keypress, &game);
    mlx_hook(game.win, 6, 1L<<6, handle_mouse_move, &game);
    mlx_hook(game.win, 17, 0, handle_close, &game);
    
    // Center the mouse initially
    mlx_mouse_move(game.mlx, game.win, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    // Set up game loop
    mlx_loop_hook(game.mlx, game_loop, &game);
    
    // Start game loop
    mlx_loop(game.mlx);

    return (0);
} 