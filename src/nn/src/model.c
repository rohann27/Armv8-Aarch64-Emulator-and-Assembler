#include "model.h"

static double rand_weight(void) {
    return ((double)rand() / RAND_MAX - 0.5);
}

static double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

static double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

static void softmax(double *z, int size) {
    double max = z[0];
    for (int i = 1; i < size; ++i) if (z[i] > max) max = z[i];

    double sum = 0;
    for (int i = 0; i < size; ++i) {
        z[i] = exp(z[i] - max);
        sum += z[i];
    }
    for (int i = 0; i < size; ++i) z[i] /= sum;
}

static void alloc_layer(layer *layer, int size, int prev_size) {
    layer->size = size;
    layer->activations = calloc(size, sizeof(double));
    layer->z_values = calloc(size, sizeof(double));
    layer->biases = malloc(size * sizeof(double));
    layer->deltas = calloc(size, sizeof(double));
    layer->grad_biases = calloc(size, sizeof(double));

    assert(layer->activations != NULL);
    assert(layer->z_values != NULL);
    assert(layer->biases != NULL);
    assert(layer->deltas != NULL);
    assert(layer->grad_biases != NULL);

    layer->weights = malloc(size * sizeof(double *));
    layer->grad_weights = malloc(size * sizeof(double *));

    assert(layer->weights != NULL);
    assert(layer->grad_weights != NULL);

    // Allocate the weights
    for (int i = 0; i < size; ++i) {
        layer->weights[i] = malloc(prev_size * sizeof(double));
        layer->grad_weights[i] = calloc(prev_size, sizeof(double));

        assert(layer->weights[i] != NULL);
        assert(layer->grad_weights[i] != NULL);

        // Initialize the weights to a random weight
        for (int j = 0; j < prev_size; ++j)
            layer->weights[i][j] = rand_weight();
        layer->biases[i] = rand_weight();
    }
}

model_t *model_create(int hidden_sizes[], int num_hidden_layers) {
    model_t *model = malloc(sizeof(model_t));
    assert(model != NULL);

    model->num_hidden_layers = num_hidden_layers;

    model->input = malloc(INPUT_SIZE * sizeof(double)); // ✅ Required!
    assert(model->input != NULL);

    model->hidden_layers = malloc(num_hidden_layers * sizeof(layer));
    assert(model->hidden_layers != NULL);

    int prev_size = INPUT_SIZE;
    for (int i = 0; i < num_hidden_layers; i++) {
        alloc_layer(&model->hidden_layers[i], hidden_sizes[i], prev_size);
        prev_size = hidden_sizes[i];
    }

    alloc_layer(&model->output_layer, OUTPUT_SIZE, prev_size);
    assert(model->input != NULL);
    return model;
}

void model_forward(model_t *model, double input[INPUT_SIZE]) {

    if (!model) {
        printf("model is NULL!\n");
        exit(1);
    }
    if (!input) {
        printf("input is NULL!\n");
        exit(1);
    }

    if (!model->input) {
        printf("model input is NULL!\n");
        exit(1);
    }

    memcpy(model->input, input, sizeof(double) * INPUT_SIZE);


    const double *prev = model->input;
    int prev_size = INPUT_SIZE;

    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *layer = &model->hidden_layers[l];

        if (!layer->biases || !layer->weights || !layer->activations) {
            printf("layer %d not initialized correctly\n", l);
            exit(1);
        }

        for (int i = 0; i < layer->size; ++i) {
            double sum = layer->biases[i];
            for (int j = 0; j < prev_size; ++j)
                sum += layer->weights[i][j] * prev[j]; // Wx + b
            layer->z_values[i] = sum;
            layer->activations[i] = sigmoid(sum);
        }
        prev = layer->activations;
        prev_size = layer->size;
    }

    layer *out = &model->output_layer;
    for (int i = 0; i < out->size; ++i) {
        double sum = out->biases[i];
        for (int j = 0; j < prev_size; ++j)
            sum += out->weights[i][j] * prev[j];
        out->z_values[i] = sum;
        out->activations[i] = sum;
    }
    softmax(out->activations, out->size);
}

void model_backward(model_t *model, double target[OUTPUT_SIZE]) {
    layer *out = &model->output_layer;
    const double *prev_activ = model->num_hidden_layers ?
        model->hidden_layers[model->num_hidden_layers - 1].activations : model->input;
    int prev_size = model->num_hidden_layers ?
        model->hidden_layers[model->num_hidden_layers - 1].size : INPUT_SIZE;

    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        double error = out->activations[i] - target[i];
        out->deltas[i] = error;
        out->grad_biases[i] = error;
        for (int j = 0; j < prev_size; ++j)
            out->grad_weights[i][j] = error * prev_activ[j];
    }

    for (int l = model->num_hidden_layers - 1; l >= 0; --l) {
        layer *layer_cur = &model->hidden_layers[l];
        layer *next = (l == model->num_hidden_layers - 1) ? out : &model->hidden_layers[l + 1];
        double *prev_act = (l == 0) ? model->input : model->hidden_layers[l - 1].activations;
        int prev_size = (l == 0) ? INPUT_SIZE : model->hidden_layers[l - 1].size;

        for (int i = 0; i < layer_cur->size; ++i) {
            double sum = 0.0;
            for (int k = 0; k < next->size; ++k)
                sum += next->weights[k][i] * next->deltas[k];
            layer_cur->deltas[i] = sigmoid_derivative(layer_cur->activations[i]) * sum;
            layer_cur->grad_biases[i] = layer_cur->deltas[i];
            for (int j = 0; j < prev_size; ++j)
                layer_cur->grad_weights[i][j] = layer_cur->deltas[i] * prev_act[j];
        }
    }
}

void model_update(model_t *model, double lr) {
    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *layer = &model->hidden_layers[l];
        int prev_size = (l == 0) ? INPUT_SIZE : model->hidden_layers[l - 1].size;
        for (int i = 0; i < layer->size; ++i) {
            layer->biases[i] -= lr * layer->grad_biases[i];
            for (int j = 0; j < prev_size; ++j)
                layer->weights[i][j] -= lr * layer->grad_weights[i][j];
        }
    }

    layer *out = &model->output_layer;
    int prev_size = (model->num_hidden_layers > 0) ?
        model->hidden_layers[model->num_hidden_layers - 1].size : INPUT_SIZE;
    for (int i = 0; i < OUTPUT_SIZE; ++i) {
        out->biases[i] -= lr * out->grad_biases[i];
        for (int j = 0; j < prev_size; ++j)
            out->weights[i][j] -= lr * out->grad_weights[i][j];
    }
}

int model_predict(model_t *model) {
    int max_i = 0;
    double max_val = model->output_layer.activations[0];
    for (int i = 1; i < OUTPUT_SIZE; ++i) {
        if (model->output_layer.activations[i] > max_val) {
            max_val = model->output_layer.activations[i];
            max_i = i;
        }
    }
    return max_i;
}

double model_compute_loss(double output[OUTPUT_SIZE], double target[OUTPUT_SIZE]) {
    double loss = 0.0;
    for (int i = 0; i < OUTPUT_SIZE; ++i)
        if (target[i] > 0) loss -= log(output[i] + 1e-10);
    return loss;
}

void model_free(model_t *model) {
    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *layer = &model->hidden_layers[l];
        for (int i = 0; i < layer->size; ++i) {
            free(layer->weights[i]);
            free(layer->grad_weights[i]);
        }
        free(layer->weights);
        free(layer->grad_weights);
        free(layer->activations);
        free(layer->z_values);
        free(layer->biases);
        free(layer->grad_biases);
        free(layer->deltas);
    }
    free(model->hidden_layers);

    layer *out = &model->output_layer;
    for (int i = 0; i < out->size; ++i) {
        free(out->weights[i]);
        free(out->grad_weights[i]);
    }
    free(out->weights);
    free(out->grad_weights);
    free(out->activations);
    free(out->z_values);
    free(out->biases);
    free(out->grad_biases);
    free(out->deltas);

    free(model->input);
    free(model);
}

void print_model_summary(model_t *model) {
    printf("===== MODEL SUMMARY =====\n");
    printf("Input size: %d\n", INPUT_SIZE);
    printf("Number of hidden layers: %d\n", model->num_hidden_layers);

    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *lay = &model->hidden_layers[l];
        printf("\tHidden Layer %d: size = %d\n", l, lay->size);
    }

    printf("Output layer size: %d\n", model->output_layer.size);

    int total_weights = 0;
    int prev_size = INPUT_SIZE;
    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *lay = &model->hidden_layers[l];
        total_weights += lay->size * prev_size;
        prev_size = lay->size;
    }
    total_weights += model->output_layer.size * prev_size;

    printf("Total number of weights: %d\n", total_weights);
    printf("=========================\n");
}
