#include "utils.h"


void apply_blur(uint8_t digit_img[28][28]) {
    uint8_t temp[28][28];

    for (int y = 0; y < 28; ++y) {
        for (int x = 0; x < 28; ++x) {
            int sum = 0;
            int count = 0;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < 28 && ny >= 0 && ny < 28) {
                        sum += digit_img[ny][nx];
                        count++;
                    }
                }
            }

            temp[y][x] = sum / count;
        }
    }

    // Ccpy back
    for (int y = 0; y < 28; ++y)
        for (int x = 0; x < 28; ++x)
            digit_img[y][x] = temp[y][x];
}



void center_digit(uint8_t digit_img[28][28]) {
    int min_x = 28, max_x = -1, min_y = 28, max_y = -1;

    // Compute bounding box
    for (int y = 0; y < 28; ++y) {
        for (int x = 0; x < 28; ++x) {
            if (digit_img[y][x] < 255)  { // not blank 
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }

    int dx = ((28 - (max_x - min_x + 1)) / 2) - min_x;
    int dy = ((28 - (max_y - min_y + 1)) / 2) - min_y;

    uint8_t temp[28][28];
    for (int y = 0; y < 28; ++y)
        for (int x = 0; x < 28; ++x)
            temp[y][x] = 255;  // clear

    for (int y = 0; y < 28; ++y) {
        for (int x = 0; x < 28; ++x) {
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < 28 && ny >= 0 && ny < 28) {
                temp[ny][nx] = digit_img[y][x];
            }
        }
    }

    // copy back
    for (int y = 0; y < 28; ++y)
        for (int x = 0; x < 28; ++x)
            digit_img[y][x] = temp[y][x];
}


void write_digit_to_file(int pred, const char *fname) {
    FILE *f = fopen(fname, "w");
    if (f) {
        fprintf(f, "%d\n", pred);
        fclose(f);
        printf("Prediction saved to prediction.txt");
    } else {
        printf("Failed to save prediction.txt");
    }
}
