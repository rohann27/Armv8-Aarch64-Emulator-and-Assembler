#include "evaluate.h"

void evaluate_on_dataset(model_t *model, dataset *data) {
    double acc = evaluate_accuracy(model, data);
    printf("Test Accuracy: %.2f%%\n", acc);

    int conf_mat[10][10];
    compute_confusion_matrix(model, data, conf_mat);
    print_confusion_matrix(conf_mat);
    save_gallery_png(data, 5, "gallery.png");
}

double evaluate_accuracy(model_t *model, dataset *test_data) {
    int correct = 0;
    for (int i = 0; i < test_data->size; ++i) {
        model_forward(model, test_data->images[i]);
        int prediction = model_predict(model);
        if (prediction == test_data->labels[i])
            correct++;
    }
    return 100.0 * correct / test_data->size;
}

void compute_confusion_matrix(model_t *model, dataset *data, int matrix[10][10]) {
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            matrix[i][j] = 0;

    for (int i = 0; i < data->size; ++i) {
        model_forward(model, data->images[i]);
        int predicted = model_predict(model);
        int true_label = data->labels[i];
        matrix[true_label][predicted]++;
    }
}

void print_confusion_matrix(int matrix[10][10]) {
    printf("Confusion Matrix:\n");
    printf("      ");
    for (int j = 0; j < 10; ++j) {
        printf(" %3d ", j);
    }
    printf("\n");

    for (int i = 0; i < 10; ++i) {
        printf(" %3d: ", i);
        for (int j = 0; j < 10; ++j) {
            printf(" %3d ", matrix[i][j]);
        }
        printf("\n");
    }
}

