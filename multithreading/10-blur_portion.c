#include "multithreading.h"

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
 * compute_pixel - Compute the blurred value of a single pixel, skipping
 *                 kernel taps that fall outside the image and renormalizing
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
	long int half, i, j, x, y;
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
			x = (long int)px + j - half;
			y = (long int)py + i - half;
			if (x < 0 || y < 0 || x >= (long int)img->w ||
				y >= (long int)img->h)
				continue;
			weight = kernel->matrix[i][j];
			p = img->pixels[(y * (long int)img->w) + x];
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
