NAME := cub3D

# Paths
SRC_PATH := src/
INC_PATH := includes/
OBJ_PATH := .obj/
LIB_PATH := libft/
MLX_PATH := minilibx-linux/
#MLX_PATH := minilibx_opengl_20191021/

# Compiler and flags
CC := cc
CFLAGS := -Wall -Werror -Wextra -g
IFLAGS := -I $(INC_PATH) -I $(LIB_PATH) -I $(MLX_PATH)
LFLAGS := -L $(LIB_PATH) -lft -L $(MLX_PATH) -lmlx_Linux -L/usr/lib -I $(MLX_PATH) -lXext -lX11 -lm -lz
#LFLAGS := -L $(LIB_PATH) -lft -L $(MLX_PATH) -lmlx -framework OpenGL -framework AppKit

# Valgrind flags
VALGRIND := valgrind
VFLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

# Source files
SUB_PATH := .

FILES := main\
         parser\
         render

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
		echo "Error: MLX library not found"; \
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

# Valgrind rule
valgrind: $(NAME)
	$(VALGRIND) $(VFLAGS) ./$(NAME) maps/1.cub

.PHONY: all clean fclean re mclean mfclean mre valgrind
