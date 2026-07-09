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
 * compute_pixel - Compute the blurred value of a single pixel, leaving it
 *                 unchanged if the kernel does not fully fit in the image
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
	long int half, extra, i, j;
	pixel_t p;
	size_t x, y;

	half = (long int)kernel->size / 2;
	extra = (long int)kernel->size - 1 - half;
	if ((long int)px < half || (long int)py < half ||
		px + (size_t)extra >= img->w || py + (size_t)extra >= img->h)
		return (img->pixels[(py * img->w) + px]);

	sum_r = 0;
	sum_g = 0;
	sum_b = 0;
	sum_k = 0;
	for (i = 0; i < (long int)kernel->size; i++)
	{
		for (j = 0; j < (long int)kernel->size; j++)
		{
			weight = kernel->matrix[i][j];
			x = (size_t)((long int)px + j - half);
			y = (size_t)((long int)py + i - half);
			p = img->pixels[(y * img->w) + x];
			sum_r += p.r * weight;
			sum_g += p.g * weight;
			sum_b += p.b * weight;
			sum_k += weight;
		}
	}
	p.r = clamp_channel(sum_r / sum_k);
	p.g = clamp_channel(sum_g / sum_k);
	p.b = clamp_channel(sum_b / sum_k);
	return (p);
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
