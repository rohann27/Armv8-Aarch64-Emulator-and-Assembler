#ifndef MODEL_IO_H
#define MODEL_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "model.h"

/**
 * Saves a trained model to a binary file.
 *
 * The file stores number of hidden layers, sizes of each layer,
 * and weights and biases for each layer.
 *
 * @param model Pointer to the model to save.
 * @param filename Path to output file.
 * @return true on success, false on failure.
 */
bool model_save(model_t *model, const char *filename);

/**
 * Loads a model from a previously saved binary file.
 *
 * Allocates a new model and restores architecture (layer sizes), 
 * weights and biases
 *
 * @param filename Path to model file.
 * @return Pointer to loaded model (must be freed with model_free), or NULL on failure.
 */
model_t *model_load(const char *filename);

#endif
