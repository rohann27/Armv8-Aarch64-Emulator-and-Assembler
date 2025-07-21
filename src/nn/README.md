# Neural Network for detecting MNIST Images

This module implements a simple fully-connected NN in pure C, aimed to classify handwritten MNIST digits (0-9).
It includes:

* Dynamic architecture (arbitrary hidden layers)
* Forward and backward propagation
* SGD training loop with progress bar
* Save/load model to file
* Predict 28x28 PNG images

## Project Structure

```
model.h           -- Core neural net model (forward, backward, update)
model_io.h        -- Save/load model from file
data_loader.h     -- Load MNIST IDX3/IDX1 data
utils.h        -- Progress bar, PNG loader
train.h

model.c
model_io.c
data_loader.c
train.c           -- Training loop
utils.c

main.c                -- Example app: train / evaluate / predict
```

## How to Use the NN Module
### Core Types:

```c
typedef struct {
    int size;
    double *activations;
    double *z_values;
    double *biases;
    double **weights;
    double *deltas;
    double **grad_weights;
    double *grad_biases;
} layer;

typedef struct {
    int num_hidden_layers;
    layer *hidden_layers;
    layer output_layer;
    double *input;
} model_t;
```
### Key Functions:

#### Model lifecycle:
```c
model_t *model_create(const int hidden_sizes[], int num_hidden_layers);
void model_free(model_t *model);
```

#### Forward pass:

```c
void model_forward(model_t *model, const double input[INPUT_SIZE]);
int model_predict(model_t *model);
double model_compute_loss(const double output[OUTPUT_SIZE], const double target[OUTPUT_SIZE]);
```

#### Backward pass and update:
```c
void model_backward(model_t *model, const double target[OUTPUT_SIZE]);
void model_update(model_t *model, double learning_rate);
```

#### Save and load model:

```c
bool model_save(model_t *model, const char *filename);
model_t *model_load(const char *filename);
```

#### Train the model:

```c
void train(model_t *model, dataset *train_data, int epochs, double learning_rate);
```

#### Evaluate accuracy:

```c
double evaluate_accuracy(model_t *model, dataset *test_data);
```

#### Data Loader:

```c
dataset *load_dataset(const char *image_file, const char *label_file);
void split_dataset(dataset *original, float test_ratio, dataset **train_set, dataset **test_set);
void free_dataset(dataset *set);
```

#### Utilities:

```c
void display_progress(int current, int total, int bar_width);
bool load_png_image(const char *filename, double output[784]);
```

### Example: Predict a PNG image

```c
model_t *model = model_load("mnist_model.bin");

double input[784];
if (load_png_image("digit.png", input)) {
    model_forward(model, input);
    int prediction = model_predict(model);
    printf("Predicted digit: %d\n", prediction);
}

model_free(model);
```

### Example: Training a model

```c
dataset *full = load_dataset("train-images.idx3-ubyte", "train-labels.idx1-ubyte");
dataset *train, *test;
split_dataset(full, 0.2, &train, &test);

int hidden[] = {128, 64};
model_t *model = model_create(hidden, 2);

train(model, train, 10, 0.01);
model_save(model, "mnist_model.bin");

model_free(model);
free_dataset(train);
free_dataset(test);
```

## Build Instructions
```
make
./bin/mnist_nn
```

## Notes

* You can use any number of hidden layers and any sizes.
* Uses SGD (stochastic gradient descent).
* Training progress is shown per epoch with a terminal progress bar.
* Model files can be saved/loaded and reused for fast inference.
