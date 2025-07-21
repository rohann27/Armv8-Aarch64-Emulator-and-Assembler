#include <stdbool.h>
#include "utils.h"
#include "../nn/src/model.h"
#include "../nn/src/model_io.h"
#include "../nn/src/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>

#include <SDL2/SDL.h>


/* DEFINITIONS FOR THE GRAPHICS */
#define GRID_SIZE  28
#define CELL_SIZE  20
#define CANVAS_W   (GRID_SIZE * CELL_SIZE)  /* 500  */
#define SIDEBAR_W  200
#define WIN_W      (CANVAS_W + SIDEBAR_W)   /* 700  */
#define WIN_H      CANVAS_W                 /* 500  */
#define BRUSH_COLOR 255
#define TARGET_FPS 60.0
#define FRAME_TIME_MS ((uint32_t)(1000.0 / TARGET_FPS))
#define SLIGHT_MARK_RATE 30
#define SLIGHT_MARKS_ACTIVATE false
#define PREDICT_FRAME_RATE 60 /* predict the result every this many frames */


#define BUTTON_X (CANVAS_W + 35)
#define BUTTON_Y 150
#define BUTTON_W 130
#define BUTTON_H 50

/* paint a solid line of cells from (x0,y0) to (x1,y1) */
static void paint_line(Uint8 grid[GRID_SIZE][GRID_SIZE],
                       int x0, int y0, int x1, int y1,
                       Uint8 colour)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int steps = (dx > dy ? dx : dy);          /* length on major axis */

    for (int i = 0; i <= steps; ++i) {
        int x = x0 + (x1 - x0) * i / steps;   /* integer lerp */
        int y = y0 + (y1 - y0) * i / steps;
        grid[y][x] = colour;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <model_file>\n", argv[0]);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return 1;
    }
    char *model_file = argv[1];

    model_t *model = model_load(model_file);
    if (!model) {
        SDL_Log("Failed to load model!");
        return 1;
    }
        SDL_Window  *win = SDL_CreateWindow("MNIST Grid Painter",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            WIN_W, WIN_H, 0);
        SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

        // set the grid to white at start
        uint8_t grid[GRID_SIZE][GRID_SIZE];
        for (int y = 0; y < GRID_SIZE; ++y)
            for (int x = 0; x < GRID_SIZE; ++x)
                grid[y][x] = 255; 

        Uint8 brush   = 0; // set brush colour to black
        bool  running = true;
        SDL_Event ev;
        int prevX = 0, prevY = 0;
        int predictFrameCount = 0;
        int count = 0;

        while (running) {
            /* -------------- frame set -------------- */
            uint32_t frame_start = SDL_GetTicks();

            uint8_t frame_elapsed = SDL_GetTicks() - frame_start;

            if (frame_elapsed < FRAME_TIME_MS)
                SDL_Delay(FRAME_TIME_MS - frame_elapsed);

            /* -------------- do prediction -------------- */
            if (predictFrameCount == 0) {
                // Copy grid to a copy of it to not change
                // the actual pantining grid
                uint8_t grid_cpy[28][28];
                memcpy(grid_cpy, grid, 28 * 28);

                // Apply preprocessing to make the grid similar
                // to training images -- see dataset in `../nn/data`
                center_digit(grid_cpy);
                apply_blur(grid_cpy);
                
                // Turn the 28x28 2d array grid to 1d array, scale
                // and make the bg black with digits white to
                // make it resemble MNIST-style digits 
                double image[INPUT_SIZE];
                uint8_t output[INPUT_SIZE];
                for (int i = 0; i < GRID_SIZE; ++i) {
                    for (int j = 0; j < GRID_SIZE; ++j) {
                        // image[i * GRID_SIZE + j] = (1 - grid_cpy[j][i] / 255.0);
                        output[i * GRID_SIZE + j] = 255 - grid_cpy[i][j];
                        image[i * GRID_SIZE + j] = output[i * GRID_SIZE + j] / 255.0;
                    }
                }

                // Save images to directory
                char fname[20];
                sprintf(fname, "digits/digit%d.png", count++);
                printf("fname: %s\n", fname);
                save_grid_to_png(output, fname);

                // Actually predict now :skull: :)
                model_forward(model, image);
                int prediction = model_predict(model);
                const char *fname_digit = "digit";
                write_digit_to_file(prediction, fname_digit);
                SDL_Log("Predicted digit: %d", prediction);
            }

            /* -------------- events -------------- */
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT)
                    running = false;

                // X/x clears the canvas --> this is better than having a button imo
                if (ev.type == SDL_KEYDOWN &&
                (ev.key.keysym.sym == SDLK_x)) {
                    for (int y = 0; y < GRID_SIZE; ++y)
                        for (int x = 0; x < GRID_SIZE; ++x)
                            grid[y][x] = 255;
                }

                // scroll-wheel adjusts brush grey 
                if (ev.type == SDL_MOUSEWHEEL) {
                    int v = (int)brush + ev.wheel.y * 10;
                    if (v < 0)   v = 0;
                    if (v > 255) v = 255;
                    brush = (Uint8)v;
                }
            }

            /* -------------- painting (drag) -------------- */
            int mx, my; uint32_t buttons = SDL_GetMouseState(&mx, &my);
            bool in_canvas = (mx < CANVAS_W && my < WIN_H);

            if (in_canvas) {
                int gx = mx / CELL_SIZE;
                int gy = my / CELL_SIZE;
                if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) && (prevX != gx || prevY != gy)) {
                    // MAKE BRUSH SIZE 3x3 instead of one cell to, again,
                    // make it closer to MNIST-style
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            int nx = gx + dx;
                            int ny = gy + dy;
                            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                                grid[ny][nx] = brush;
                            }
                        }
                    }
                    
                    if (SLIGHT_MARKS_ACTIVATE) {
                        if (gy < GRID_SIZE - 1) {
                            grid[gy + 1][gx] = (grid[gy + 1][gx] > SLIGHT_MARK_RATE) ? grid[gy + 1][gx] - SLIGHT_MARK_RATE : 0;
                        }
                        if (gy > 0) {
                            grid[gy - 1][gx] = (grid[gy - 1][gx] > SLIGHT_MARK_RATE) ? grid[gy - 1][gx] - SLIGHT_MARK_RATE : 0;
                        }
                        if (gx < GRID_SIZE - 1) {
                            grid[gy][gx + 1] = (grid[gy][gx + 1] > SLIGHT_MARK_RATE) ? grid[gy][gx + 1] - SLIGHT_MARK_RATE : 0;
                        }
                        if (gx > 0) {
                            grid[gy][gx - 1] = (grid[gy][gx - 1] > SLIGHT_MARK_RATE) ? grid[gy][gx - 1] - SLIGHT_MARK_RATE : 0;
                        }
                    }

                    if (prevX != -1 && prevY != -1) {
                        paint_line(grid, gx, gy, prevX, prevY, 0);
                    }

                    prevX = gx;
                    prevY = gy;
                }
                else {
                    prevX = -1;
                    prevY = -1;
                }

                if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
                    grid[gy][gx] = 0;
            }

            /* -------------- draw -------------- */
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderClear(ren);

            /* cells */
            for (int y = 0; y < GRID_SIZE; ++y)
                for (int x = 0; x < GRID_SIZE; ++x) {
                    uint8_t g = grid[y][x];
                    SDL_SetRenderDrawColor(ren, g, g, g, 255);
                    SDL_Rect r = { x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE };
                    SDL_RenderFillRect(ren, &r);
                }

            /* grid lines */
            SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
            for (int i = 0; i <= GRID_SIZE; ++i) {
                SDL_RenderDrawLine(ren, i*CELL_SIZE, 0, i*CELL_SIZE, WIN_H);
                SDL_RenderDrawLine(ren, 0, i*CELL_SIZE, CANVAS_W, i*CELL_SIZE);
            }

            /* sidebar */
            SDL_SetRenderDrawColor(ren, 30, 30, 30, 255);
            SDL_Rect bar = { CANVAS_W, 0, SIDEBAR_W, WIN_H };
            SDL_RenderFillRect(ren, &bar);

            /* brush preview */
            SDL_SetRenderDrawColor(ren, brush,brush,brush,255);
            SDL_Rect prev = { CANVAS_W+35, 35, 80, 80 };
            SDL_RenderFillRect(ren, &prev);

            SDL_RenderPresent(ren);
            SDL_Delay(16);

            predictFrameCount++;
            if (predictFrameCount == PREDICT_FRAME_RATE) predictFrameCount = 0;
        }

        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 0;
}
