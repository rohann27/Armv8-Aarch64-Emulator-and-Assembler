#ifndef NN_UTILS_H
#define NN_UTILS_H

#include <stdio.h>
#include <stdbool.h>

#include "data_loader.h"

/**
 * Displays a progress bar in the terminal.
 *
 * @param current Current iteration (e.g., sample index).
 * @param total Total number of iterations (e.g., total samples).
 * @param bar_width Width of the progress bar in characters (e.g., 50).
 */
void display_progress(int current, int total, int bar_width);

/**
 * Loads a grayscale PNG image (with expected size: 28x28), 
 * normalizes pixel values to [0,1] and stores them in a 
 * 784-element array suitable for predicting.
 *
 * @param filename Path to PNG image file.
 * @param output Output array of 784 doubles, normalized to [0,1].
 * @return true if successful, false if failed
 */
bool load_png_image(const char *filename, double output[784]);

/**
 * Save a single 28x28 grayscale image to PNG file.
 * The resulting PNG will be exactly 28x28 pixels, grayscale.
 *
 * @param pixels Flat array of 28x28 grayscale pixels (uint8_t[784]).
 * @param filename Path to output PNG file.
 * @return true on success, false on failure.
 */
bool save_grid_to_png(uint8_t pixels[28 * 28], char *filename);

/**
 * Save a gallery PNG showing multiple sample images per digit.
 *
 * Selects up to N images for each digit (0-9) from the dataset and arranges them
 * into a single gallery PNG with:
 * - 10 rows (one per digit)
 * - samples_per_digit columns per row
 *
 * The gallery image is saved as a PNG for easy visualization of the dataset.
 *
 * @param data Dataset containing images and labels.
 * @param samples_per_digit Number of samples to display for each digit (0-9).
 * @param filename Path to output PNG file.
 */
void save_gallery_png(dataset *data, int samples_per_digit, const char *filename);

#endif
