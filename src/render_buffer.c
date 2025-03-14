/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_buffer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:15:54 by kokaimov          #+#    #+#             */
/*   Updated: 2025/03/14 22:13:16 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	init_buffer(t_img *img, t_buffer *buf)
{
	buf->addr = img->addr;
	buf->bytes_per_pixel = img->bits_per_pixel / 8;
	buf->line_length = img->line_length;
	buf->width = WINDOW_WIDTH;
	buf->height = WINDOW_HEIGHT;
}

static void	swap_buffer_pixels(t_buffer *buf)
{
	int				x;
	int				y;
	unsigned int	temp;
	t_pixel_addr	pixel;

	y = 0;
	while (y < buf->height)
	{
		x = 0;
		while (x < buf->width / 2)
		{
			pixel.left = buf->addr + (y * buf->line_length
					+ x * buf->bytes_per_pixel);
			pixel.right = buf->addr + (y * buf->line_length + 
					(buf->width - 1 - x) * buf->bytes_per_pixel);
			temp = *(unsigned int *)pixel.left;
			*(unsigned int *)pixel.left = *(unsigned int *)pixel.right;
			*(unsigned int *)pixel.right = temp;
			x++;
		}
		y++;
	}
}

void	flip_buffer_horizontally(t_img *img)
{
	t_buffer	buf;

	init_buffer(img, &buf);
	swap_buffer_pixels(&buf);
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
