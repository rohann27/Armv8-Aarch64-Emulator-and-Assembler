#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

// Size of MNIST input images (28 x 28 = 784 pixels)
#define IMAGE_SIZE 784

// Number of output classes (digits 0-9)
#define NUM_CLASSES 10

/**
 * Represents a collection of images and their corresponding labels
 *
 * Each image is stored as an array of 784 doubles (normalized grayscale [0,1]),
 * and labels are stored as `uint8_t` digits (0 to 9).
 *
 * The images array is of size `size`, with each `images[i]` being a `double[784]` array.
 */
typedef struct {
    double **images;    // Normalized [0,1]
    uint8_t *labels;    // Raw digit labels (0-9)
    int size;
} dataset;

/**
 * Loads MNIST image file (IDX format) and returns an array of normalized images.
 *
 * @param filename Path to IDX3 image file.
 * @param images Output pointer to allocated 2D array of normalized images.
 * @param count Output number of images loaded.
 * @return 0 on success, -1 on error.
 */
int read_mnist_images(const char *filename, double ***images, int *count);

/**
 * Loads MNIST label file (IDX format) and returns an array of labels.
 *
 * @param filename Path to IDX1 label file.
 * @param labels Output pointer to allocated array of labels.
 * @param count Output number of labels loaded.
 * @return 0 on success, -1 on error.
 */
int read_mnist_labels(const char *filename, uint8_t **labels, int *count);

/**
 * Loads a dataset from MNIST IDX files (images + labels).
 *
 * @param image_file Path to IDX3 image file.
 * @param label_file Path to IDX1 label file.
 * @return Pointer to allocated dataset structure.
 */
dataset *load_dataset(const char *image_file, const char *label_file);

/**
 * Save a dataset to files in standard MNIST IDX format.
 *
 * Saves images and labels in the same binary format as the original MNIST dataset:
 *
 * - images_file: IDX3 format
 * - labels_file: IDX1 format
 *
 * Allows you to export any dataset (train/test/split) to IDX files
 * which can later be reloaded by `load_dataset()`
 *
 * NOTE THAT!!!!
 * - The dataset images are assumed to be normalized to [0.0, 1.0] and will be rescaled to [0, 255].
 *
 * @param data Dataset to save.
 * @param images_file Path to output IDX3-ubyte image file.
 * @param labels_file Path to output IDX1-ubyte label file.
 * @return true on success, false on error.
 */
bool save_dataset_to_idx(dataset *data, const char *images_file, const char *labels_file);

/**
 * Splits a dataset into training and testing sets.
 * 
 * NOTE!!! The original dataset should be freed manually after splitting.
 *
 * @param original The original dataset to split.
 * @param test_ratio Fraction of data to put in test set (e.g. 0.2 = 20% test).
 * @param train_set Output pointer to allocated training set.
 * @param test_set Output pointer to allocated test set.
 */
void split_dataset(dataset *original, float test_ratio, dataset **train_set, dataset **test_set);

// Free dataset 
void free_dataset(dataset *set);

/**
 * Converts a label into one-hot encoded array, where
 * the label is the index of the only nonzero element of
 * the one hot encoded array.
 *
 * @param label The label (0-9).
 * @param output Output array of NUM_CLASSES doubles (one-hot encoded).
 */
void one_hot_encode(uint8_t label, double output[NUM_CLASSES]);

#endif
