#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

// Size of input and output layers
#define INPUT_SIZE 784 // 28 x 28
#define OUTPUT_SIZE 10 // digits from 0 to 9

/**
 * Represents a fully connected layer in the neural network.
 *
 * Contains weights, biases, activations, gradients for backpropagation
 */
typedef struct {
    int size;                       // Number of neurons in this layer
    double *activations;            // Output activations for this layer, f(z)
    double *z_values;               // Pre-activation values (Wx + b)
    double *biases;                 // Biases for each nuron in this layer
    
    double **weights;               // Weight matrix: [size][prev_size]
    double *deltas;                 // Backpropagation deltas
    double **grad_weights;          // Gradient of weights
    double *grad_biases;            // Gradient of biases
} layer;

/**
 * Represents the entire neural network model.
 *
 * Contains: input layer, N hidden layers, 1 output layer, and input
 */
typedef struct {
    int num_hidden_layers;
    layer *hidden_layers;

    layer output_layer;

    double *input; // INPUT_SIZE
} model_t;

/**
 * @Creates a new model with the specified architecture.
 *
 * @param hidden_sizes Array of sizes for each hidden layer.
 * @param num_hidden_layers Number of hidden layers.
 * @return Pointer to newly allocated model.
 */
model_t *model_create(int hidden_sizes[], int num_hidden_layers);

// Free the memory allocated by the model
void model_free(model_t *model);

/**
 * Performs forward propagation on the model.
 *
 * @param model Pointer to the model.
 * @param input Input image, flattened and normalized.
 */
void model_forward(model_t *model, double input[INPUT_SIZE]);

/**
 * Performs backward propagation (computes gradients).
 *
 * @param model Pointer to the model.
 * @param target One-hot target vector (size = OUTPUT_SIZE).
 */
void model_backward(model_t *model, double target[OUTPUT_SIZE]);

/**
 * Updates model parameters using gradients and learning rate.
 *
 * @param model Pointer to the model.
 * @param learning_rate Learning rate for gradient descent.
 */
void model_update(model_t *model, double learning_rate);

/**
 * Returns predicted label from current output layer.
 *
 * @param model Pointer to the model.
 * @return Predicted label (0-9).
 */
int model_predict(model_t *model);

/**
 * Computes categorical cross-entropy loss between model output and target.
 *
 * $$L = - \sum_{i=1}^c y_i \cdot \log(p_i)$$
 * 
 * @param output Current model output (after softmax).
 * @param target One-hot target vector.
 * @return Cross-entropy loss value.
 */
double model_compute_loss(double output[OUTPUT_SIZE], double target[OUTPUT_SIZE]);

// For debug, print summary of the model.
void print_model_summary(model_t *model);

#endif 
