#include "data_loader.h"

// Read 4 bytes from the given file in big endian
// style. MNIST IDX files stores integers in big-endian
// format so we have to read in big endian style
static int read_big_endian_4(FILE *f) {
    uint8_t bytes[4];
    fread(bytes, 1, 4, f);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// Again, MNIST files are stored in big-endian format
static void write_big_endian_4(FILE *f, int val)
{
    uint8_t bytes[4];
    bytes[0] = (val >> 24) & 0xFF;
    bytes[1] = (val >> 16) & 0xFF;
    bytes[2] = (val >> 8) & 0xFF;
    bytes[3] = val & 0xFF;
    fwrite(bytes, 1, 4, f);
}



int read_mnist_images(const char *filename, double ***images_out, int *count_out) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    int magic = read_big_endian_4(f);
    assert(magic == 2051);

    int count = read_big_endian_4(f);
    int rows = read_big_endian_4(f);
    int cols = read_big_endian_4(f);
    assert(rows == 28 && cols == 28);

    double **images = malloc(count * sizeof(double *));
    for (int i = 0; i < count; ++i) {
        images[i] = malloc(IMAGE_SIZE * sizeof(double));
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            uint8_t pixel;
            fread(&pixel, 1, 1, f);
            images[i][j] = pixel / 255.0;  // normalize
        }
    }

    fclose(f);
    *images_out = images;
    *count_out = count;
    return 0;
}

int read_mnist_labels(const char *filename, uint8_t **labels_out, int *count_out) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    int magic = read_big_endian_4(f);
    assert(magic == 2049);

    int count = read_big_endian_4(f);
    uint8_t *labels = malloc(count * sizeof(uint8_t));
    fread(labels, 1, count, f);
    fclose(f);

    *labels_out = labels;
    *count_out = count;
    return 0;
}

dataset *load_dataset(const char *image_file, const char *label_file) {
    dataset *set = malloc(sizeof(dataset));
    if (read_mnist_images(image_file, &set->images, &set->size) != 0) {
        fprintf(stderr, "Failed to load images\n");
        exit(EXIT_FAILURE);
    }

    int label_count;
    if (read_mnist_labels(label_file, &set->labels, &label_count) != 0 || label_count != set->size) {
        fprintf(stderr, "Failed to load labels or mismatch\n");
        exit(EXIT_FAILURE);
    }

    return set;
}

bool save_dataset_to_idx(dataset *data, const char *images_file, const char *labels_file)
{
    // Write images
    FILE *f_img = fopen(images_file, "wb");
    if (!f_img) {
        fprintf(stderr, "Failed to open %s for writing\n", images_file);
        return false;
    }

    write_big_endian_4(f_img, 2051);           // magic number
    write_big_endian_4(f_img, data->size);     // number of images
    write_big_endian_4(f_img, 28);             // rows
    write_big_endian_4(f_img, 28);             // cols

    for (int i = 0; i < data->size; ++i) {
        for (int j = 0; j < 28 * 28; ++j) {
            uint8_t pix = (uint8_t)(data->images[i][j] * 255.0 + 0.5);  // scale back to 0-255
            fwrite(&pix, 1, 1, f_img);
        }
    }

    fclose(f_img);

    // Write labels
    FILE *f_lbl = fopen(labels_file, "wb");
    if (!f_lbl) {
        fprintf(stderr, "Failed to open %s for writing\n", labels_file);
        return false;
    }

    write_big_endian_4(f_lbl, 2049);          // magic number
    write_big_endian_4(f_lbl, data->size);    // number of labels

    fwrite(data->labels, 1, data->size, f_lbl);

    fclose(f_lbl);

    printf("Saved dataset: %d images\n", data->size);
    printf(" -> %s\n", images_file);
    printf(" -> %s\n", labels_file);

    return true;
}


void split_dataset(dataset *original, float test_ratio, dataset **train_set, dataset **test_set) {
    int total = original->size;
    int test_size = (int)(total * test_ratio);
    int train_size = total - test_size;

    *train_set = malloc(sizeof(dataset));
    *test_set = malloc(sizeof(dataset));

    (*train_set)->size = train_size;
    (*test_set)->size = test_size;

    (*train_set)->images = malloc(train_size * sizeof(double *));
    (*train_set)->labels = malloc(train_size * sizeof(uint8_t));
    (*test_set)->images = malloc(test_size * sizeof(double *));
    (*test_set)->labels = malloc(test_size * sizeof(uint8_t));

    for (int i = 0; i < total; ++i) {
        if (i < train_size) {
            (*train_set)->images[i] = original->images[i];
            (*train_set)->labels[i] = original->labels[i];
        } else {
            (*test_set)->images[i - train_size] = original->images[i];
            (*test_set)->labels[i - train_size] = original->labels[i];
        }
    }
}

void one_hot_encode(uint8_t label, double output[NUM_CLASSES]) {
    for (int i = 0; i < NUM_CLASSES; ++i) output[i] = 0.0;
    output[label] = 1.0;
}

void free_dataset(dataset *set) {
    for (int i = 0; i < set->size; ++i)
        free(set->images[i]);
    free(set->images);
    free(set->labels);
    free(set);
}
