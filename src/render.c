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

void    draw_vertical_line(t_game *game, int x, int draw_start, int draw_end, int tex_x, t_texture *tex, double perp_wall_dist)
{
    int y;
    int tex_y;
    int color;
    char *tex_data;
    int tex_bpp;
    int tex_line_size;
    int tex_endian;
    double step;
    double tex_pos;
    static int debug_count = 0;
    int line_height = draw_end - draw_start;

    // Make sure texture_x is within bounds of texture width
    tex_x = tex_x % tex->width;
    if (tex_x < 0)
        tex_x += tex->width;
        
    tex_data = mlx_get_data_addr(tex->img, &tex_bpp, &tex_line_size, &tex_endian);
    
    // Debug info for one line every 60 frames
    if (debug_count++ % 60 == 0 && x == WINDOW_WIDTH / 2)
    {
        printf("\n=== DEBUG: Vertical Line Drawing ===\n");
        printf("Drawing at x=%d\n", x);
        printf("Draw range: %d to %d (height: %d)\n", draw_start, draw_end, draw_end - draw_start);
        printf("Wall distance: %f\n", perp_wall_dist);
        printf("Texture X: %d\n", tex_x);
        printf("Texture size: %dx%d\n", tex->width, tex->height);
    }
    
    // Calculate how much to increase the texture coordinate per screen pixel
    step = 1.0 * tex->height / line_height;
    
    // For very close walls, start from the middle of the texture and show a zoomed portion
    if (perp_wall_dist < 0.5)
    {
        // Calculate center of the texture
        double center_tex_y = tex->height / 2.0;
        
        // Calculate zoom factor based on distance (closer = more zoom)
        double zoom_factor = 0.5 / perp_wall_dist;
        if (zoom_factor > 5.0) zoom_factor = 5.0; // Limit maximum zoom
        
        // Calculate visible texture height (smaller = more zoom)
        double visible_tex_height = tex->height / zoom_factor;
        
        // Calculate starting texture position
        tex_pos = center_tex_y - (visible_tex_height / 2.0);
        
        // Recalculate step
        step = visible_tex_height / line_height;
        
        if (debug_count % 60 == 1 && x == WINDOW_WIDTH / 2)
        {
            printf("Close wall! Zoom factor: %f\n", zoom_factor);
            printf("Visible texture height: %f\n", visible_tex_height);
            printf("Starting tex_pos: %f\n", tex_pos);
        }
    }
    else
    {
        // For normal distances, start from the top of the texture
        tex_pos = 0;
    }
    
    y = draw_start;
    while (y < draw_end)
    {
        // Use modulo for safe texture wrapping
        tex_y = ((int)tex_pos) % tex->height;
        if (tex_y < 0)
            tex_y += tex->height;
        
        if (tex_x >= 0 && tex_x < tex->width && tex_y >= 0 && tex_y < tex->height)
        {
            color = *(unsigned int*)(tex_data + (tex_y * tex_line_size + tex_x * (tex_bpp / 8)));
            draw_pixel(&game->img, x, y, color);
        }
        
        tex_pos += step;
        y++;
    }
}

void    render_3d_view(t_game *game)
{
    double camera_x;
    double ray_dir_x;
    double ray_dir_y;
    int map_x;
    int map_y;
    double side_dist_x;
    double side_dist_y;
    double delta_dist_x;
    double delta_dist_y;
    double perp_wall_dist;
    int step_x;
    int step_y;
    int hit;
    int side;
    int line_height;
    int draw_start;
    int draw_end;
    t_texture *tex;
    int tex_x;
    double wall_x;
    static int debug_count = 0;

    // Draw ceiling
    for (int y = 0; y < WINDOW_HEIGHT / 2; y++)
        for (int x = 0; x < WINDOW_WIDTH; x++)
            draw_pixel(&game->img, x, y, 
                (game->config.ceiling.r << 16) | 
                (game->config.ceiling.g << 8) | 
                game->config.ceiling.b);

    // Draw floor
    for (int y = WINDOW_HEIGHT / 2; y < WINDOW_HEIGHT; y++)
        for (int x = 0; x < WINDOW_WIDTH; x++)
            draw_pixel(&game->img, x, y, 
                (game->config.floor.r << 16) | 
                (game->config.floor.g << 8) | 
                game->config.floor.b);

    // Raycasting loop
    for (int x = 0; x < WINDOW_WIDTH; x++)
    {
        camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
        ray_dir_x = game->player.dir_x + game->player.plane_x * camera_x;
        ray_dir_y = game->player.dir_y + game->player.plane_y * camera_x;

        // Debug middle ray every 60 frames
        if (debug_count++ % 60 == 0 && x == WINDOW_WIDTH / 2)
        {
            printf("\n=== DEBUG: Ray Casting Info ===\n");
            printf("Ray direction: (%f, %f)\n", ray_dir_x, ray_dir_y);
            printf("Camera plane: (%f, %f)\n", game->player.plane_x, game->player.plane_y);
            printf("Camera X: %f\n", camera_x);
        }

        map_x = (int)game->player.x;
        map_y = (int)game->player.y;

        delta_dist_x = fabs(1 / ray_dir_x);
        delta_dist_y = fabs(1 / ray_dir_y);

        hit = 0;
        
        if (ray_dir_x < 0)
        {
            step_x = -1;
            side_dist_x = (game->player.x - map_x) * delta_dist_x;
        }
        else
        {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - game->player.x) * delta_dist_x;
        }
        if (ray_dir_y < 0)
        {
            step_y = -1;
            side_dist_y = (game->player.y - map_y) * delta_dist_y;
        }
        else
        {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - game->player.y) * delta_dist_y;
        }

        // DDA algorithm
        while (hit == 0)
        {
            if (side_dist_x < side_dist_y)
            {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }
            if (game->config.map.grid[map_y][map_x] == '1')
                hit = 1;
        }

        if (side == 0)
            perp_wall_dist = (side_dist_x - delta_dist_x);
        else
            perp_wall_dist = (side_dist_y - delta_dist_y);

        // Calculate line height based on distance
        line_height = (int)(WINDOW_HEIGHT / perp_wall_dist);
        
        // Calculate lowest and highest pixel to fill in current stripe
        draw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_start < 0)
            draw_start = 0;
        draw_end = line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_end >= WINDOW_HEIGHT)
            draw_end = WINDOW_HEIGHT - 1;

        // Calculate texture coordinates
        if (side == 0)
        {
            wall_x = game->player.y + perp_wall_dist * ray_dir_y;
            if (ray_dir_x > 0)
                tex = &game->config.east;
            else
                tex = &game->config.west;
        }
        else
        {
            wall_x = game->player.x + perp_wall_dist * ray_dir_x;
            if (ray_dir_y > 0)
                tex = &game->config.south;
            else
                tex = &game->config.north;
        }
        
        // Take only fractional part of wall_x
        wall_x -= floor(wall_x);
        
        // Calculate x coordinate on the texture
        tex_x = (int)(wall_x * tex->width);
        
        // Flip texture x coordinate if needed
        if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0))
            tex_x = tex->width - tex_x - 1;

        if (debug_count % 60 == 1 && x == WINDOW_WIDTH / 2)
        {
            printf("\n=== DEBUG: Wall Hit Info ===\n");
            printf("Wall hit at: (%d, %d)\n", map_x, map_y);
            printf("Perpendicular wall distance: %f\n", perp_wall_dist);
            printf("Line height: %d\n", line_height);
            printf("Draw range: %d to %d\n", draw_start, draw_end);
            printf("Wall X: %f\n", wall_x);
            printf("Texture X: %d\n", tex_x);
            printf("Texture size: %dx%d\n", tex->width, tex->height);
            printf("Side hit: %s\n", side == 0 ? "Vertical" : "Horizontal");
        }

        // Pass the perpendicular wall distance to the drawing function
        draw_vertical_line(game, x, draw_start, draw_end, tex_x, tex, perp_wall_dist);
    }
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