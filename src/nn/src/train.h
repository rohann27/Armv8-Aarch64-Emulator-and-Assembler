#ifndef TRAIN_H
#define TRAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "model.h"
#include "data_loader.h"
#include "utils.h"

/**
 * @Trains the model on a given training dataset using gradient descent.
 *
 * For each epoch:
 * - Shuffles training data.
 * - Performs forward pass, backward pass, and updates weights for each sample.
 * - Prints progress and current loss/accuracy.
 *
 * @param model Pointer to model to train.
 * @param train_data Pointer to training dataset.
 * @param epochs Number of training epochs.
 * @param learning_rate Learning rate for gradient descent.
 */
void train(model_t *model, dataset *train_data, int epochs, double learning_rate);

#endif
