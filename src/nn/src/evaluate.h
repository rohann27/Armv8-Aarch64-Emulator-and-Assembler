#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdio.h>

#include "utils.h"
#include "model.h"
#include "data_loader.h"

/**
 * Evaluate a trained model on a dataset and print performance metrics.
 *
 * Computes and prints:
 * - overall test accuracy
 * - confusion matrix (10x10)
 * - saves a gallery of example images (gallery.png)
 *
 * @param model Pointer to trained model.
 * @param data Dataset to evaluate (e.g. test set).
 */
void evaluate_on_dataset(model_t *model, dataset *data);

/**
 * Evaluates the model accuracy on a given test dataset.
 *
 * Performs forward propagation for each test image, compares 
 * the predicted label to the true label, and computes overall
 * classification accuracy.
 *
 * @param model Pointer to trained model.
 * @param test_data Pointer to test dataset.
 * @return Accuracy as a percentage (0.0 to 100.0).
 */
double evaluate_accuracy(model_t *model, dataset *test_data);

/**
 * Compute the confusion matrix of the model on a dataset.
 * The matrix is stored as:
 *  `matrix[true_label][predicted_label]`
 *
 * @param model Pointer to trained model.
 * @param data Dataset to evaluate.
 * @param matrix Output 10x10 confusion matrix.
 */
void compute_confusion_matrix(model_t *model, dataset *data, int matrix[10][10]);

void print_confusion_matrix(int matrix[10][10]);

#endif
