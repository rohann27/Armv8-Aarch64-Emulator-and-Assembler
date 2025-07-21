#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>   

#include "model.h"
#include "data_loader.h"
#include "train.h"
#include "model_io.h"
#include "evaluate.h"
#include "utils.h"


#define EPOCHS 10
#define LEARNING_RATE 0.01


void train_pipeline(model_t *model, const char *train_imgs, const char *train_labels, int epochs, double lr, char *model_out) {
    dataset *full = load_dataset(train_imgs, train_labels);
    dataset *train_set;
    dataset *test_set;
    split_dataset(full, 0.2, &train_set, &test_set);
    printf("Saving test set...\n");
    save_dataset_to_idx(test_set, "data/test-images.idx3-ubyte", "data/test-labels.idx1-ubyte");

    printf("========= TRAINING MODEL =========\n");
    printf("Parameters:\n");
    printf("\tNumber of epochs: %d\n", epochs);
    printf("\tLearning rate: %.3f\n", lr);
    train(model, train_set, epochs, lr);
    printf("==================================\n");
    model_save(model, model_out);
    printf("========= TESTING MODEL =========\n");
    printf("Parameters:\n");
    printf("\tTest dataset size: %d\n", test_set->size);
    evaluate_on_dataset(model, test_set);
    printf("==================================\n");
    free_dataset(full);
}

void test_pipeline(model_t *model, const  char *img_path, const char *label_path, const char *mod_path) {
    dataset *test_set = load_dataset(img_path, label_path);
    printf("========= TESTING MODEL =========\n");
    printf("Using model: %s\n", mod_path);
    printf("Data file: %s, %s\n", img_path, label_path);
    printf("Parameters:\n");
    printf("\tTest dataset size: %d\n", test_set->size);
    evaluate_on_dataset(model, test_set);
    printf("==================================\n");
    free_dataset(test_set);
}

void predict_pipeline(model_t *model, const char *image_file) {
    double image[INPUT_SIZE];
    if (!load_png_image(image_file, image)) {
        printf("Failed to load image: %s\n", image_file);
        model_free(model);
        exit(EXIT_FAILURE);
    }
    model_forward(model, image);
    int prediction = model_predict(model);
    printf("Predicted digit: %d\n", prediction);
}

/*
mnist_nn train <img_path> <label_path> <out_path> {epochs} {lr}
mnist_nn predict <model_path> <img>
mnist_nn test <model_path> <img_path> <label_path>
 */ 
int main(int argc, char **argv) {
    srand(time(NULL));
    if (argc < 4) {
        printf("Usage:\n");
        printf("\t%s train <training_img_path> <training_label_path> <model_out_path>\n", argv[0]);
        printf("\t%s predict <model_path> <sample_png>\n", argv[0]);
        return EXIT_FAILURE;
    }
    model_t *model = NULL;
    if (strcmp(argv[1], "train") == 0) {
        int epochs = EPOCHS;
        double lr = LEARNING_RATE;
        if (argc == 7) {
            epochs = atoi(argv[5]);
            lr = atof(argv[6]);
        }

        int hidden[] = {128, 128};  
        model = model_create(hidden, 2);
        print_model_summary(model);
        char *train_imgs = argv[2];
        char *train_labels = argv[3];
        char *model_out = argv[4];
        train_pipeline(model, train_imgs, train_labels, epochs, lr, model_out);
    }
    else if (strcmp(argv[1], "predict") == 0) {
        const char *model_file = argv[2];
        const char *image_file = argv[3];

        model = model_load(model_file);
        if (!model) {
            printf("Failed to load model\n");
            return 1;
        }
        predict_pipeline(model, image_file);
    }
    else if (strcmp(argv[1], "test") == 0) {
        const char *mod_path = argv[2];
        const char *img_path = argv[3];
        const char *label_path = argv[4];

        model = model_load(mod_path);
        if (!model) {
            printf("Failed to load model\n");
            return 1;
        }
        test_pipeline(model, img_path, label_path, mod_path);
    }
    model_free(model);
    return 0;
}
