#include "train.h"


static void shuffle_indices(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

void train(model_t *model, dataset *train_data,
           int epochs, double learning_rate) {
    int n = train_data->size;
    int *indices = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { indices[i] = i; }

    double target[NUM_CLASSES];
    for (int epoch = 0; epoch < epochs; ++epoch) {
        shuffle_indices(indices, n);
        double total_loss = 0.0;
        int correct = 0;

        for (int i = 0; i < n; ++i) {
            int idx = indices[i];
            double *input = train_data->images[idx];
            uint8_t label = train_data->labels[idx];

            one_hot_encode(label, target);

            model_forward(model, input);
            total_loss += model_compute_loss(model->output_layer.activations, target);
        
            int predicted = model_predict(model);
            if (predicted == label) correct++;

            model_backward(model, target);
            model_update(model, learning_rate);
            if (i % (n / 100) == 0 || i == n - 1) { 
                display_progress(i + 1, n, 50);
            }
        }
        double train_acc = 100.0 * correct / n;
        double avg_loss = total_loss / n;

        printf("Epoch %2d/%d | Loss: %.4f | Train Acc: %.2f%%\n",
               epoch + 1, epochs, avg_loss, train_acc);
    }

    free(indices);
}

