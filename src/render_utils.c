/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madvil2 <madvil2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023-11-15 12:00:00 by madvil2           #+#    #+#             */
/*   Updated: 2023-11-15 12:00:00 by madvil2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	draw_pixel(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
	{
		dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
		*(unsigned int *)dst = color;
	}
}

void	draw_rectangle(t_img *img, int x, int y, int size, int color)
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

void	flip_buffer_horizontally(t_img *img)
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

void	render_3d_view(t_game *game)
{
	render_ceiling_floor(game);
	render_walls(game);
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