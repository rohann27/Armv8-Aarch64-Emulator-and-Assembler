#include "model_io.h"

bool model_save(model_t *model, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return false;

    fwrite(&model->num_hidden_layers, sizeof(int), 1, f);

    // Write layer sizes
    for (int i = 0; i < model->num_hidden_layers; ++i) {
        int size = model->hidden_layers[i].size;
        fwrite(&size, sizeof(int), 1, f);
    }

    // Write weights & biases for hidden layers
    int prev_size = INPUT_SIZE;
    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *layer = &model->hidden_layers[l];
        for (int i = 0; i < layer->size; ++i) {
            fwrite(layer->weights[i], sizeof(double), prev_size, f);
            fwrite(&layer->biases[i], sizeof(double), 1, f);
        }
        prev_size = layer->size;
    }

    // Write weights & biases for output layer
    layer *out = &model->output_layer;
    for (int i = 0; i < out->size; ++i) {
        fwrite(out->weights[i], sizeof(double), prev_size, f);
        fwrite(&out->biases[i], sizeof(double), 1, f);
    }

    fclose(f);
    return true;
}

model_t *model_load(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    int num_hidden;
    fread(&num_hidden, sizeof(int), 1, f);

    int *hidden_sizes = malloc(num_hidden * sizeof(int));
    for (int i = 0; i < num_hidden; ++i)
        fread(&hidden_sizes[i], sizeof(int), 1, f);

    model_t *model = model_create(hidden_sizes, num_hidden);
    free(hidden_sizes);

    // Read weights & biases for hidden layers
    int prev_size = INPUT_SIZE;
    for (int l = 0; l < model->num_hidden_layers; ++l) {
        layer *layer = &model->hidden_layers[l];
        for (int i = 0; i < layer->size; ++i) {
            fread(layer->weights[i], sizeof(double), prev_size, f);
            fread(&layer->biases[i], sizeof(double), 1, f);
        }
        prev_size = layer->size;
    }

    // Read weights & biases for output layer
    layer *out = &model->output_layer;
    for (int i = 0; i < out->size; ++i) {
        fread(out->weights[i], sizeof(double), prev_size, f);
        fread(&out->biases[i], sizeof(double), 1, f);
    }

    fclose(f);
    return model;
}


