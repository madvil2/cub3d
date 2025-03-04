#include "cub3d.h"

#define MAP_SCALE 16  // Size of each map cell in pixels
#define PLAYER_SIZE 4 // Size of player dot in pixels

void    draw_pixel(t_img *img, int x, int y, int color)
{
    char    *dst;

    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

void    draw_rectangle(t_img *img, int x, int y, int size, int color)
{
    int i;
    int j;

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

void    draw_player(t_game *game, int map_x, int map_y)
{
    int center_x;
    int center_y;
    int dir_x;
    int dir_y;
    int i;

    // Draw player position
    center_x = map_x + (int)(game->player.x * MAP_SCALE);
    center_y = map_y + (int)(game->player.y * MAP_SCALE);
    
    // Draw player dot
    i = -PLAYER_SIZE/2;
    while (i <= PLAYER_SIZE/2)
    {
        int j = -PLAYER_SIZE/2;
        while (j <= PLAYER_SIZE/2)
        {
            draw_pixel(&game->img, center_x + i, center_y + j, 0xFF0000);
            j++;
        }
        i++;
    }

    // Draw direction line - inverted direction for minimap
    dir_x = center_x + (int)(game->player.dir_x * MAP_SCALE);
    dir_y = center_y + (int)(game->player.dir_y * MAP_SCALE);
    
    // Simple line drawing from center to direction point
    double dx = dir_x - center_x;
    double dy = dir_y - center_y;
    double steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
    double x_inc = dx / steps;
    double y_inc = dy / steps;
    double x = center_x;
    double y = center_y;
    
    i = 0;
    while (i <= steps)
    {
        draw_pixel(&game->img, (int)x, (int)y, 0xFF0000);
        x += x_inc;
        y += y_inc;
        i++;
    }
}

void    flip_minimap(t_game *game, int map_x, int map_y, int width, int height)
{
    int x, y;
    int half_width = width / 2;

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < half_width; x++)
        {
            // Get colors of pixels to swap
            unsigned int left_color, right_color;
            left_color = *(unsigned int*)(game->img.addr + 
                ((y + map_y) * game->img.line_length + (x + map_x) * (game->img.bits_per_pixel / 8)));
            right_color = *(unsigned int*)(game->img.addr + 
                ((y + map_y) * game->img.line_length + (width - 1 - x + map_x) * (game->img.bits_per_pixel / 8)));

            // Swap pixels
            draw_pixel(&game->img, x + map_x, y + map_y, right_color);
            draw_pixel(&game->img, width - 1 - x + map_x, y + map_y, left_color);
        }
    }
}

void    render_map(t_game *game)
{
    int x;
    int y;
    int map_x;
    int map_y;
    int color;
    int map_width;
    int map_height;

    // Calculate map position and dimensions
    map_x = 10;  // 10 pixels from left edge
    map_y = 10;  // 10 pixels from top edge
    map_width = game->config.map.width * MAP_SCALE;
    map_height = game->config.map.height * MAP_SCALE;

    // Draw map grid
    y = 0;
    while (y < game->config.map.height)
    {
        x = 0;
        while (x < game->config.map.width)
        {
            if (game->config.map.grid[y][x] == '1')
                color = 0xFFFFFF;  // White for walls
            else if (game->config.map.grid[y][x] == '0' || 
                    game->config.map.grid[y][x] == 'N' ||
                    game->config.map.grid[y][x] == 'S' ||
                    game->config.map.grid[y][x] == 'E' ||
                    game->config.map.grid[y][x] == 'W')
                color = 0x404040;  // Dark gray for floor
            else
                color = 0x000000;  // Black for empty space

            draw_rectangle(&game->img, 
                         map_x + x * MAP_SCALE, 
                         map_y + y * MAP_SCALE, 
                         MAP_SCALE - 1,  // -1 to create grid effect
                         color);
            x++;
        }
        y++;
    }

    // Draw player position and direction
    draw_player(game, map_x, map_y);

    // Flip the minimap area horizontally
    flip_minimap(game, map_x, map_y, map_width, map_height);
}

void    load_texture(t_game *game, t_texture *texture)
{
    int width;
    int height;

    texture->img = mlx_xpm_file_to_image(game->mlx, texture->path, &width, &height);
    if (!texture->img)
        return ;
    texture->width = width;
    texture->height = height;
}

void    load_textures(t_game *game)
{
    load_texture(game, &game->config.north);
    load_texture(game, &game->config.south);
    load_texture(game, &game->config.east);
    load_texture(game, &game->config.west);
}

static double calculate_step(t_texture *tex, int line_height, double perp_wall_dist)
{
    double step;
    double visible_portion;
    
    if (perp_wall_dist < 1.0)
    {
        double zoom_factor = 1.0 / perp_wall_dist;
        if (zoom_factor > 20.0) 
            zoom_factor = 20.0;
        visible_portion = 1.0 / zoom_factor;
        step = (visible_portion * tex->height) / line_height;
    }
    else
    {
        step = (double)tex->height / line_height;
    }
    return (step);
}

static double calculate_tex_pos(t_texture *tex, double perp_wall_dist, int draw_start)
{
    double tex_pos;
    
    if (perp_wall_dist < 1.0)
    {
        double zoom_factor = 1.0 / perp_wall_dist;
        if (zoom_factor > 20.0) 
            zoom_factor = 20.0;
        double visible_portion = 1.0 / zoom_factor;
        double player_height_ratio = 0.5;
        double center_of_tex = tex->height * player_height_ratio;
        
        tex_pos = center_of_tex - ((visible_portion * tex->height) * player_height_ratio);
        double window_offset_ratio = (double)draw_start / WINDOW_HEIGHT;
        tex_pos += window_offset_ratio * visible_portion * tex->height;
    }
    else
    {
        tex_pos = 0;
    }
    return (tex_pos);
}

static void validate_tex_coords(int *tex_x, int *tex_y, t_texture *tex)
{
    if (*tex_x >= tex->width)
        *tex_x = tex->width - 1;
    if (*tex_x < 0)
        *tex_x = 0;
    if (*tex_y >= tex->height)
        *tex_y = tex->height - 1;
    if (*tex_y < 0)
        *tex_y = 0;
}

void draw_vertical_line(t_game *game, int x, int draw_start, int draw_end, 
                        int tex_x, t_texture *tex, double perp_wall_dist)
{
    int y;
    int tex_y;
    int color;
    double step;
    double tex_pos;
    
    validate_tex_coords(&tex_x, &tex_y, tex);
    step = calculate_step(tex, draw_end - draw_start, perp_wall_dist);
    tex_pos = calculate_tex_pos(tex, perp_wall_dist, draw_start);
    
    char *tex_data = mlx_get_data_addr(tex->img, &game->img.bits_per_pixel, 
                                     &game->img.line_length, &game->img.endian);
    
    y = draw_start;
    while (y < draw_end)
    {
        tex_y = (int)tex_pos;
        validate_tex_coords(&tex_x, &tex_y, tex);
        color = *(unsigned int*)(tex_data + (tex_y * game->img.line_length + 
                                tex_x * (game->img.bits_per_pixel / 8)));
        draw_pixel(&game->img, x, y, color);
        tex_pos += step;
        y++;
    }
}

static void init_ray(t_game *game, double camera_x, double *ray_dir_x, double *ray_dir_y)
{
    *ray_dir_x = game->player.dir_x + game->player.plane_x * camera_x;
    *ray_dir_y = game->player.dir_y + game->player.plane_y * camera_x;
}

static void calculate_delta_dist(double ray_dir_x, double ray_dir_y, 
                               double *delta_dist_x, double *delta_dist_y)
{
    *delta_dist_x = fabs(1 / ray_dir_x);
    *delta_dist_y = fabs(1 / ray_dir_y);
}

static void setup_dda(int *map_x, int *map_y, double ray_dir_x, double ray_dir_y,
                    double *side_dist_x, double *side_dist_y,
                    double delta_dist_x, double delta_dist_y,
                    int *step_x, int *step_y, t_player *player)
{
    *map_x = (int)player->x;
    *map_y = (int)player->y;
    
    if (ray_dir_x < 0)
    {
        *step_x = -1;
        *side_dist_x = (player->x - *map_x) * delta_dist_x;
    }
    else
    {
        *step_x = 1;
        *side_dist_x = (*map_x + 1.0 - player->x) * delta_dist_x;
    }
    
    if (ray_dir_y < 0)
    {
        *step_y = -1;
        *side_dist_y = (player->y - *map_y) * delta_dist_y;
    }
    else
    {
        *step_y = 1;
        *side_dist_y = (*map_y + 1.0 - player->y) * delta_dist_y;
    }
}

static void perform_dda(int *map_x, int *map_y, int step_x, int step_y,
                     double *side_dist_x, double *side_dist_y,
                     double delta_dist_x, double delta_dist_y,
                     int *side, t_map *map)
{
    int hit = 0;
    
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
        
        if (map->grid[*map_y][*map_x] == '1')
            hit = 1;
    }
}

static void calculate_wall_data(double ray_dir_x, double ray_dir_y,
                            int map_x, int map_y, int side,
                            double side_dist_x, double side_dist_y,
                            double delta_dist_x, double delta_dist_y,
                            t_player *player, double *perp_wall_dist,
                            double *wall_x)
{
    if (side == 0)
    {
        *perp_wall_dist = (side_dist_x - delta_dist_x);
        *wall_x = player->y + *perp_wall_dist * ray_dir_y;
    }
    else
    {
        *perp_wall_dist = (side_dist_y - delta_dist_y);
        *wall_x = player->x + *perp_wall_dist * ray_dir_x;
    }
    *wall_x -= floor(*wall_x);
}

static void calculate_line_height(double perp_wall_dist, int *line_height,
                               int *draw_start, int *draw_end)
{
    if (perp_wall_dist <= 0.1)
        perp_wall_dist = 0.1;
    
    *line_height = (int)(WINDOW_HEIGHT / perp_wall_dist);
    
    *draw_start = -*line_height / 2 + WINDOW_HEIGHT / 2;
    if (*draw_start < 0)
        *draw_start = 0;
    
    *draw_end = *line_height / 2 + WINDOW_HEIGHT / 2;
    if (*draw_end >= WINDOW_HEIGHT)
        *draw_end = WINDOW_HEIGHT - 1;
}

static void select_texture(t_game *game, int side, double ray_dir_x, double ray_dir_y,
                        double wall_x, t_texture **tex, int *tex_x)
{
    if (side == 0)
    {
        *tex = ray_dir_x > 0 ? &game->config.east : &game->config.west;
    }
    else
    {
        *tex = ray_dir_y > 0 ? &game->config.south : &game->config.north;
    }
    
    *tex_x = (int)(wall_x * (*tex)->width);
    if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0))
        *tex_x = (*tex)->width - *tex_x - 1;
}

static void draw_ceiling_floor(t_game *game)
{
    int x;
    int y;
    int ceiling_color;
    int floor_color;
    
    ceiling_color = (game->config.ceiling.r << 16) | 
                   (game->config.ceiling.g << 8) | 
                    game->config.ceiling.b;
    
    floor_color = (game->config.floor.r << 16) | 
                 (game->config.floor.g << 8) | 
                  game->config.floor.b;
    
    for (y = 0; y < WINDOW_HEIGHT / 2; y++)
        for (x = 0; x < WINDOW_WIDTH; x++)
            draw_pixel(&game->img, x, y, ceiling_color);
    
    for (y = WINDOW_HEIGHT / 2; y < WINDOW_HEIGHT; y++)
        for (x = 0; x < WINDOW_WIDTH; x++)
            draw_pixel(&game->img, x, y, floor_color);
}

void    render_3d_view(t_game *game)
{
    double ray_dir_x;
    double ray_dir_y;
    double camera_x;
    int x;
    
    // Draw ceiling and floor
    draw_ceiling_floor(game);

    for (x = 0; x < WINDOW_WIDTH; x++)
    {
        camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
        init_ray(game, camera_x, &ray_dir_x, &ray_dir_y);
        
        render_ray(game, x, ray_dir_x, ray_dir_y);
    }
}

static void render_ray(t_game *game, int x, double ray_dir_x, double ray_dir_y)
{
    int map_x, map_y, step_x, step_y;
    double side_dist_x, side_dist_y, delta_dist_x, delta_dist_y;
    
    calculate_delta_dist(ray_dir_x, ray_dir_y, &delta_dist_x, &delta_dist_y);
    setup_dda(&map_x, &map_y, ray_dir_x, ray_dir_y, &side_dist_x, &side_dist_y,
            delta_dist_x, delta_dist_y, &step_x, &step_y, &game->player);
    
    int side;
    perform_dda(&map_x, &map_y, step_x, step_y, &side_dist_x, &side_dist_y,
             delta_dist_x, delta_dist_y, &side, &game->config.map);
    
    double perp_wall_dist, wall_x;
    calculate_wall_data(ray_dir_x, ray_dir_y, map_x, map_y, side,
                      side_dist_x, side_dist_y, delta_dist_x, delta_dist_y,
                      &game->player, &perp_wall_dist, &wall_x);
    
    int line_height, draw_start, draw_end;
    calculate_line_height(perp_wall_dist, &line_height, &draw_start, &draw_end);
    
    t_texture *tex;
    int tex_x;
    select_texture(game, side, ray_dir_x, ray_dir_y, wall_x, &tex, &tex_x);
    
    draw_vertical_line(game, x, draw_start, draw_end, tex_x, tex, perp_wall_dist);
}

void    flip_buffer_horizontally(t_img *img)
{
    int x, y;
    unsigned int temp;
    char *buffer = img->addr;
    int bytes_per_pixel = img->bits_per_pixel / 8;

    for (y = 0; y < WINDOW_HEIGHT; y++)
    {
        for (x = 0; x < WINDOW_WIDTH / 2; x++)
        {
            // Calculate pixel positions
            char *left_pixel = buffer + (y * img->line_length + x * bytes_per_pixel);
            char *right_pixel = buffer + (y * img->line_length + (WINDOW_WIDTH - 1 - x) * bytes_per_pixel);
            
            // Swap pixels
            temp = *(unsigned int*)left_pixel;
            *(unsigned int*)left_pixel = *(unsigned int*)right_pixel;
            *(unsigned int*)right_pixel = temp;
        }
    }
}

void    render_frame(t_game *game)
{
    static int frame_count = 0;

    // Print debug info every 60 frames
    if (frame_count++ % 60 == 0)
    {
        printf("\n=== DEBUG: Texture Information ===\n");
        printf("North texture: %dx%d\n", game->config.north.width, game->config.north.height);
        printf("South texture: %dx%d\n", game->config.south.width, game->config.south.height);
        printf("East texture: %dx%d\n", game->config.east.width, game->config.east.height);
        printf("West texture: %dx%d\n", game->config.west.width, game->config.west.height);
        printf("TEXTURE_WIDTH: %d, TEXTURE_HEIGHT: %d\n", game->config.east.width, game->config.east.height);
        printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
        printf("Player position: (%f, %f)\n", game->player.x, game->player.y);
        printf("Player direction: (%f, %f)\n", game->player.dir_x, game->player.dir_y);
        printf("================================\n");
    }

    // Clear the image
    ft_memset(game->img.addr, 0, WINDOW_WIDTH * WINDOW_HEIGHT * (game->img.bits_per_pixel / 8));
    
    // Render the 3D view
    render_3d_view(game);
    
    // Render the minimap in the top-left corner
    render_map(game);

    // Flip the buffer horizontally
    flip_buffer_horizontally(&game->img);
    
    // Put the image to window
    mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
} 