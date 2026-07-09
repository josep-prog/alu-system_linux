#include "multithreading.h"

/**
 * get_pixel - Get a pixel from an image, clamping out of bound coordinates
 * @img: Image to read the pixel from
 * @x: X coordinate of the pixel
 * @y: Y coordinate of the pixel
 *
 * Return: The requested pixel
 */
static pixel_t get_pixel(img_t const *img, long int x, long int y)
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= (long int)img->w)
		x = (long int)img->w - 1;
	if (y >= (long int)img->h)
		y = (long int)img->h - 1;

	return (img->pixels[(y * (long int)img->w) + x]);
}

/**
 * clamp_channel - Clamp a computed channel value into the [0, 255] range
 * @v: Value to clamp
 *
 * Return: v clamped to a valid uint8_t range
 */
static uint8_t clamp_channel(float v)
{
	if (v < 0)
		return (0);
	if (v > 255)
		return (255);
	return ((uint8_t)v);
}

/**
 * compute_pixel - Compute the blurred value of a single pixel
 * @img: Source image
 * @kernel: Convolution kernel to apply
 * @px: X coordinate of the pixel to compute
 * @py: Y coordinate of the pixel to compute
 *
 * Return: The blurred pixel
 */
static pixel_t compute_pixel(img_t const *img, kernel_t const *kernel,
	size_t px, size_t py)
{
	float sum_r, sum_g, sum_b, sum_k, weight;
	long int half, i, j;
	pixel_t p, res;

	sum_r = 0;
	sum_g = 0;
	sum_b = 0;
	sum_k = 0;
	half = (long int)kernel->size / 2;

	for (i = 0; i < (long int)kernel->size; i++)
	{
		for (j = 0; j < (long int)kernel->size; j++)
		{
			weight = kernel->matrix[i][j];
			p = get_pixel(img, (long int)px + j - half,
				(long int)py + i - half);
			sum_r += p.r * weight;
			sum_g += p.g * weight;
			sum_b += p.b * weight;
			sum_k += weight;
		}
	}
	res.r = clamp_channel(sum_r / sum_k);
	res.g = clamp_channel(sum_g / sum_k);
	res.b = clamp_channel(sum_b / sum_k);
	return (res);
}

/**
 * blur_portion - Blur a portion of an image using a Gaussian Blur
 * @portion: Pointer to the portion of the image to blur
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x, y;

	for (y = portion->y; y < portion->y + portion->h; y++)
	{
		for (x = portion->x; x < portion->x + portion->w; x++)
		{
			portion->img_blur->pixels[(y * portion->img->w) + x] =
				compute_pixel(portion->img, portion->kernel, x, y);
		}
	}
}
