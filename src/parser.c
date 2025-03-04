#include "cub3d.h"

static void debug_print_map(t_map *map)
{
    int i;

    printf("\n=== DEBUG: Map ===\n");
    printf("Size: %dx%d\n", map->width, map->height);
    printf("Player position: (%d,%d) facing %c\n", 
           map->player_x, map->player_y, map->player_dir);
    for (i = 0; i < map->height; i++)
        printf("[%s]\n", map->grid[i]);
    printf("================\n\n");
}

static void debug_print_config(t_config *config)
{
    printf("\n=== DEBUG: Configuration ===\n");
    printf("Textures:\n");
    printf("  NO: %s\n", config->north.path);
    printf("  SO: %s\n", config->south.path);
    printf("  WE: %s\n", config->west.path);
    printf("  EA: %s\n", config->east.path);
    printf("Colors:\n");
    printf("  Floor: RGB(%d,%d,%d)\n", 
           config->floor.r, config->floor.g, config->floor.b);
    printf("  Ceiling: RGB(%d,%d,%d)\n", 
           config->ceiling.r, config->ceiling.g, config->ceiling.b);
    printf("========================\n\n");
}

static int is_empty_line(char *line)
{
    int i;

    i = 0;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    return (line[i] == '\n' || line[i] == '\0');
}

static int parse_rgb(char *str, t_color *color)
{
    char    **split;
    int     i;

    printf("DEBUG: Parsing RGB: [%s]\n", str);
    split = ft_split(str, ',');
    if (!split)
    {
        printf("DEBUG: Error - Failed to split RGB values\n");
        return (0);
    }
    i = 0;
    while (split[i])
        i++;
    if (i != 3)
    {
        printf("DEBUG: Error - Invalid RGB format (need 3 values)\n");
        i = 0;
        while (split[i])
            free(split[i++]);
        free(split);
        return (0);
    }
    printf("DEBUG: RGB components before parsing: [%s], [%s], [%s]\n", split[0], split[1], split[2]);
    color->r = ft_atoi(split[0]);
    color->g = ft_atoi(split[1]);
    color->b = ft_atoi(split[2]);
    printf("DEBUG: RGB components after parsing: r=%d, g=%d, b=%d\n", color->r, color->g, color->b);
    i = 0;
    while (split[i])
        free(split[i++]);
    free(split);
    if (color->r < 0 || color->r > 255 || color->g < 0 || color->g > 255 
        || color->b < 0 || color->b > 255)
    {
        printf("DEBUG: Error - RGB values out of range (0-255)\n");
        return (0);
    }
    printf("DEBUG: Successfully parsed RGB: %d,%d,%d\n", color->r, color->g, color->b);
    return (1);
}

int parse_texture(char *line, t_texture *texture)
{
    int i;

    printf("DEBUG: Parsing texture path: [%s]\n", line);
    i = 0;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    texture->path = ft_strdup(&line[i]);
    if (!texture->path)
    {
        printf("DEBUG: Error - Failed to allocate texture path\n");
        return (0);
    }
    i = ft_strlen(texture->path) - 1;
    if (texture->path[i] == '\n')
        texture->path[i] = '\0';
    printf("DEBUG: Successfully parsed texture path: [%s]\n", texture->path);
    return (1);
}

int parse_color(char *line, t_color *color)
{
    int i;
    int j;
    char *trimmed;
    int ret;

    printf("DEBUG: Parsing color line: [%s]\n", line);
    // Skip leading spaces
    i = 0;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    
    // Find end of color values
    j = ft_strlen(line);
    // Trim trailing spaces and newline
    while (j > i && (line[j - 1] == ' ' || line[j - 1] == '\t' || line[j - 1] == '\n'))
        j--;
    
    // Create trimmed string
    trimmed = ft_substr(line, i, j - i);
    if (!trimmed)
        return (0);
    
    printf("DEBUG: Color value after trimming: [%s]\n", trimmed);
    ret = parse_rgb(trimmed, color);
    free(trimmed);
    return (ret);
}

static int is_map_line(char *line)
{
    int i;

    i = 0;
    while (line[i] && (line[i] == ' ' || line[i] == '\t'))
        i++;
    return (line[i] == '1' || line[i] == '0' || 
            line[i] == 'N' || line[i] == 'S' || 
            line[i] == 'E' || line[i] == 'W');
}

static int is_valid_map_char(char c)
{
    return (c == '0' || c == '1' || c == 'N' || c == 'S' || 
            c == 'E' || c == 'W' || c == ' ' || c == '\n');
}

static int validate_map(t_map *map)
{
    int i;
    int j;
    int player_count;

    player_count = 0;
    i = 0;
    while (i < map->height)
    {
        j = 0;
        while (j < map->width && map->grid[i][j])
        {
            if (!is_valid_map_char(map->grid[i][j]))
                return (0);
            if (map->grid[i][j] == 'N' || map->grid[i][j] == 'S' ||
                map->grid[i][j] == 'E' || map->grid[i][j] == 'W')
            {
                player_count++;
                map->player_x = j;
                map->player_y = i;
                map->player_dir = map->grid[i][j];
            }
            // Check if empty space is surrounded by walls
            if (map->grid[i][j] == '0' || map->grid[i][j] == 'N' ||
                map->grid[i][j] == 'S' || map->grid[i][j] == 'E' ||
                map->grid[i][j] == 'W')
            {
                if (i == 0 || i == map->height - 1 || j == 0 || j == map->width - 1 ||
                    map->grid[i-1][j] == ' ' || map->grid[i+1][j] == ' ' ||
                    map->grid[i][j-1] == ' ' || map->grid[i][j+1] == ' ')
                    return (0);
            }
            j++;
        }
        i++;
    }
    return (player_count == 1);
}

static char **collect_map_lines(char *first_line, int fd)
{
    char **lines;
    char *line;
    int i;

    lines = malloc(sizeof(char *) * 1000); // Temporary size, will be resized
    if (!lines)
        return (NULL);
    lines[0] = ft_strdup(first_line);
    i = 1;
    while ((line = get_next_line(fd)) != NULL)
    {
        if (is_empty_line(line))
        {
            // If we already have map lines and encounter an empty line, it's an error
            if (i > 0)
            {
                printf("DEBUG: Error - Empty line within map\n");
                free(line);
                while (--i >= 0)
                    free(lines[i]);
                free(lines);
                return (NULL);
            }
        }
        else
        {
            lines[i] = ft_strdup(line);
            i++;
        }
        free(line);
    }
    lines[i] = NULL;
    return (lines);
}

static int process_map(t_game *game, char **map_lines)
{
    t_map *map;
    int i;
    int j;
    int max_width;

    if (!map_lines)  // Add check for NULL map_lines
        return (0);

    map = &game->config.map;
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

    i = 0;
    while (map_lines[i])
    {
        free(map_lines[i]);
        i++;
    }
    free(map_lines);

    return (validate_map(map));
}

int parse_config(t_game *game, char *file_path)
{
    int     fd;
    char    *line;
    int     ret;
    int     params_set;
    int     i;

    printf("\nDEBUG: Starting to parse config file: %s\n", file_path);
    fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        printf("DEBUG: Error - Cannot open file\n");
        return (0);
    }

    ft_memset(&game->config, 0, sizeof(t_config));
    game->config.floor.r = -1;  // Initialize to invalid value
    game->config.ceiling.r = -1;  // Initialize to invalid value
    params_set = 0;

    while ((line = get_next_line(fd)) != NULL)
    {
        ret = 1;
        printf("\nDEBUG: Processing line: [%s]", line);
        
        // Skip leading spaces
        i = 0;
        while (line[i] && (line[i] == ' ' || line[i] == '\t'))
            i++;
            
        if (ft_strncmp(line + i, "NO ", 3) == 0)
            ret = parse_texture(line + i + 3, &game->config.north);
        else if (ft_strncmp(line + i, "SO ", 3) == 0)
            ret = parse_texture(line + i + 3, &game->config.south);
        else if (ft_strncmp(line + i, "WE ", 3) == 0)
            ret = parse_texture(line + i + 3, &game->config.west);
        else if (ft_strncmp(line + i, "EA ", 3) == 0)
            ret = parse_texture(line + i + 3, &game->config.east);
        else if (ft_strncmp(line + i, "F ", 2) == 0)
        {
            ret = parse_color(line + i + 2, &game->config.floor);
            printf("DEBUG: Floor color set to RGB(%d,%d,%d)\n", 
                   game->config.floor.r, game->config.floor.g, game->config.floor.b);
        }
        else if (ft_strncmp(line + i, "C ", 2) == 0)
        {
            ret = parse_color(line + i + 2, &game->config.ceiling);
            printf("DEBUG: Ceiling color set to RGB(%d,%d,%d)\n", 
                   game->config.ceiling.r, game->config.ceiling.g, game->config.ceiling.b);
        }
        else if (is_map_line(line))
        {
            printf("\nDEBUG: Found start of map\n");
            if (!params_set)
            {
                printf("DEBUG: Error - Map found before all parameters were set\n");
                free(line);
                close(fd);
                return (0);
            }
            char **map_lines = collect_map_lines(line, fd);
            ret = process_map(game, map_lines);
            if (ret)
                debug_print_map(&game->config.map);
            free(line);
            break;
        }
        free(line);
        if (!ret)
        {
            printf("DEBUG: Error during parsing\n");
            break;
        }
        // Check if all parameters are set
        if (game->config.north.path && game->config.south.path &&
            game->config.west.path && game->config.east.path &&
            game->config.floor.r != -1 && game->config.ceiling.r != -1)
        {
            params_set = 1;
            debug_print_config(&game->config);
        }
    }
    close(fd);
    printf("DEBUG: Finished parsing with result: %d\n\n", ret);
    return (ret);
}

void free_config(t_config *config)
{
    int i;

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