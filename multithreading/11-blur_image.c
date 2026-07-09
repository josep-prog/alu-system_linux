#include <pthread.h>
#include "multithreading.h"

#define MAX_THREADS 16

/**
 * compute_portions - Split an image into row based portions
 * @portions: Array of portions to fill in
 * @nb: Number of portions to compute
 * @img: Source image
 * @img_blur: Destination image
 * @kernel: Convolution kernel to use
 */
static void compute_portions(blur_portion_t *portions, size_t nb,
	img_t const *img, img_t *img_blur, kernel_t const *kernel)
{
	size_t rows, extra, i, y;

	rows = img->h / nb;
	extra = img->h % nb;
	y = 0;
	for (i = 0; i < nb; i++)
	{
		portions[i].img = img;
		portions[i].img_blur = img_blur;
		portions[i].kernel = kernel;
		portions[i].x = 0;
		portions[i].w = img->w;
		portions[i].y = y;
		portions[i].h = rows + (i < extra ? 1 : 0);
		y += portions[i].h;
	}
}

/**
 * thread_blur - Thread entry point, blurs a single portion of an image
 * @arg: Address of the portion to blur
 *
 * Return: NULL
 */
static void *thread_blur(void *arg)
{
	blur_portion((blur_portion_t const *)arg);
	return (NULL);
}

/**
 * blur_image - Blur the entirety of an image using a Gaussian Blur
 * @img_blur: Address at which the blurred image is stored
 * @img: Source image
 * @kernel: Convolution kernel to use
 */
void blur_image(img_t *img_blur, img_t const *img, kernel_t const *kernel)
{
	blur_portion_t portions[MAX_THREADS];
	pthread_t threads[MAX_THREADS];
	size_t nb, i;

	if (img->h == 0)
		return;

	nb = img->h < MAX_THREADS ? img->h : MAX_THREADS;
	compute_portions(portions, nb, img, img_blur, kernel);

	for (i = 0; i < nb; i++)
		pthread_create(&threads[i], NULL, &thread_blur, &portions[i]);
	for (i = 0; i < nb; i++)
		pthread_join(threads[i], NULL);
}
