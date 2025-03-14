NAME := cub3D

# Detect OS
UNAME_S := $(shell uname -s)

# Paths
SRC_PATH := src/
INC_PATH := includes/
OBJ_PATH := .obj/
LIB_PATH := libft/

# MLX path and flags based on OS
ifeq ($(UNAME_S),Linux)
	MLX_PATH := minilibx-linux/
	LFLAGS := -L $(LIB_PATH) -lft -L $(MLX_PATH) -lmlx_Linux -L/usr/lib -I $(MLX_PATH) -lXext -lX11 -lm -lz
else ifeq ($(UNAME_S),Darwin)
	MLX_PATH := minilibx_opengl_20191021/
	LFLAGS := -L $(LIB_PATH) -lft -L $(MLX_PATH) -lmlx -framework OpenGL -framework AppKit
else
	$(error Unsupported operating system)
endif

# Compiler and flags
CC := cc
CFLAGS := -Wall -Werror -Wextra -g
IFLAGS := -I $(INC_PATH) -I $(LIB_PATH) -I $(MLX_PATH)

# Valgrind flags (only used on Linux)
VALGRIND := valgrind
VFLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

# Source files
SUB_PATH := .

FILES := game_init\
         player_movement\
         input_handling\
         game_loop\
         parser_config\
         parser_colors\
         parser_textures\
         parser_map\
         parser_map_utils_1\
         parser_map_utils_2\
         parser_validate\
         parser_utils\
         parser_memory\
         render_utils\
         render_buffer\
         render_minimap\
         render_minimap_utils\
         render_textures\
         render_raycasting\
         render_raycasting_step\
         render_raycasting_hit\
         render_raycasting_draw\
         render_walls\
         render_ceiling_floor\
         render_debug

# Libraries
MLX := $(MLX_PATH)libmlx.a
LIB := $(LIB_PATH)libft.a

# Headers, sources, objects
HDRS := $(INC_PATH)cub3d.h
SRCS := $(addsuffix .c, $(FILES))
OBJS := $(addprefix $(OBJ_PATH), $(addsuffix .o, $(FILES)))

vpath %.c $(addprefix $(SRC_PATH), $(SUB_PATH))

all: $(NAME)

$(NAME): $(MLX) $(LIB) $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $(NAME)

$(MLX):
	@if [ ! -d "$(MLX_PATH)" ]; then \
		echo "Error: MLX library not found at $(MLX_PATH)"; \
		echo "Please install the appropriate version:"; \
		echo "- For Linux: minilibx-linux"; \
		echo "- For MacOS: minilibx_opengl_20191021"; \
		exit 1; \
	fi
	$(MAKE) -C $(MLX_PATH)

$(LIB):
	@if [ ! -d "$(LIB_PATH)" ]; then \
		git clone https://github.com/madvil2/libft.git $(LIB_PATH); \
	fi
	$(MAKE) -C $(LIB_PATH)

$(OBJS): $(OBJ_PATH)%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean: mclean
	$(MAKE) clean -C $(LIB_PATH)
	$(MAKE) clean -C $(MLX_PATH)

fclean: mfclean
	$(MAKE) fclean -C $(LIB_PATH)
	$(MAKE) clean -C $(MLX_PATH)

re: fclean all

mclean:
	rm -f $(OBJS)

mfclean: mclean
	rm -f $(NAME)
	rm -rf $(OBJ_PATH)

mre: mfclean all

# Valgrind rule (Linux only)
valgrind: $(NAME)
ifeq ($(UNAME_S),Linux)
	$(VALGRIND) $(VFLAGS) ./$(NAME) maps/example.cub
else
	@echo "Valgrind is not available on macOS. Try 'leaks' instead."
endif

.PHONY: all clean fclean re mclean mfclean mre valgrind
