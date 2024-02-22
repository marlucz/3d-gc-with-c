#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define FPS 30
// how many ms each frame will take
#define FRAME_TARGET_TIME (1000 / FPS)

// extern means that this is external variable defined in the implementation
// (display.c)
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer; // -> uint32_t means that element should be of
                               // length 32bits (4 bytes)
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;

// declarations for which implementations are in .c files
bool initialize_window(void);
void draw_grid();
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int xCoord, int yCoord, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);

#endif