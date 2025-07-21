
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"

#include "utils.h"

void display_progress(int current, int total, int bar_width) {
    float progress = (float)current / total;
    int pos = (int)(bar_width * progress);

    printf("[");
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %3d%%\r", (int)(progress * 100));
    fflush(stdout);
}

bool load_png_image(const char *filename, double output[784]) {
    int width, height, channels;
    uint8_t *data = stbi_load(filename, &width, &height, &channels, 1); // force grayscale (1)

    if (!data) {
        printf("Failed to load PNG image: %s\n", filename);
        return false;
    }

    if (width != 28 || height != 28) {
        printf("Expected 28x28 image, got %dx%d\n", width, height);
        stbi_image_free(data);
        return false;
    }

    for (int i = 0; i < 784; ++i) {
        output[i] = data[i] / 255.0;
    }

    stbi_image_free(data);
    return true;
}

bool save_grid_to_png(uint8_t pixels[28 * 28], char *filename) {
    int width = 28;
    int height = 28;
    int channels = 1;  // grayscale

    int stride_in_bytes = width * channels;

    int result = stbi_write_png(filename, width, height, channels, pixels, stride_in_bytes);
    return result != 0;
}

void save_gallery_png(dataset *data, int samples_per_digit, const char *filename) {
    const int img_size = 28;
    const int num_digits = 10;
    const int cell_w = img_size;
    const int cell_h = img_size;
    const int rows = num_digits;
    const int cols = samples_per_digit;

    const int W = cols * cell_w;
    const int H = rows * cell_h;

    uint8_t *gallery = malloc(W * H);

    // Clear
    for (int i = 0; i < W * H; ++i)
        gallery[i] = 255;

    int shown[10] = {0};

    for (int i = 0; i < data->size; ++i)
    {
        uint8_t label = data->labels[i];

        if (shown[label] < samples_per_digit)
        {
            int cx = shown[label];
            int cy = label;

            // Copy image
            for (int y = 0; y < img_size; ++y)
            {
                for (int x = 0; x < img_size; ++x)
                {
                    int idx_src = y * img_size + x;
                    double pix = data->images[i][idx_src];
                    uint8_t val = (uint8_t)(255.0 * pix);  // invert: black bg, white digit

                    int gx = cx * cell_w + x;
                    int gy = cy * cell_h + y;
                    gallery[gy * W + gx] = val;
                }
            }

            shown[label]++;
        }

        // All done?
        bool done = true;
        for (int d = 0; d < 10; ++d)
            if (shown[d] < samples_per_digit)
                done = false;

        if (done)
            break;
    }

    stbi_write_png(filename, W, H, 1, gallery, W);

    printf("Saved gallery PNG: %s\n", filename);

    free(gallery);
}
