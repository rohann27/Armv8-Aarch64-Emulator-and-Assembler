#ifndef GRAPHICS_UTILS_H
#define GRAPHICS_UTILS_H

#include <stdio.h>
#include <stdint.h>

/**
 * Apply a simple blur filter to a 28x28 grayscale image.
 *
 * Performs a basic 3x3 box blur on the image, averaging each pixel with its neighbors.
 * This helps smooth out sharp edges in the digit and makes the image resemble
 * MNIST-style digits more closely.
 *
 * The image is stored in-place: digit_img[y][x]
 *
 * @param digit_img 28x28 grayscale image (uint8_t), modified in-place.
 */
void apply_blur(uint8_t digit_img[28][28]);

/**
 * Center a handwritten digit within a 28x28 image.
 *
 * Finds the bounding box of the non-blank (non-white) pixels in the image,
 * and shifts the digit horizontally and vertically so that it is centered
 * within the image canvas. The aim is to make images resemble MNIST style
 * digits more closely.
 *
 * The image is stored in-place: digit_img[y][x]
 *
 * @param digit_img 28x28 grayscale image (uint8_t), modified in-place.
 */
void center_digit(uint8_t digit_img[28][28]);

void write_digit_to_file(int pred, const char *fname);

#endif
