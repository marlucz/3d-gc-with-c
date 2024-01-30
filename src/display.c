#include "display.h"

#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // Use SDL to query what is the fullscreen max width and height
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;

  // Create SDL Window
  window = SDL_CreateWindow(NULL,                    // Window title
                            SDL_WINDOWPOS_CENTERED,  // x position
                            SDL_WINDOWPOS_CENTERED,  // y position
                            window_width,            // Window width
                            window_height,           // Window height
                            SDL_WINDOW_BORDERLESS    // no window decoration
  );
  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  // Create a SDL Renderer
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }
  // change video mode to be real fullscreen
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void draw_grid() {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      if (y % 10 == 0 || x % 10 == 0) {
        color_buffer[(window_width * y) + x] = 0xFF333333;
      }
    }
  };
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x < window_width && y < window_height) {
    color_buffer[(window_width * y) + x] = color;
  }
}

void draw_rect(int xCoord, int yCoord, int width, int height, uint32_t color) {
  // more performant way to draw rect as it's not looping over every pixel
  // calculations here just starts from the initial coordinates of the rect
  // and fill every pixel with color until rect's width and height are met
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int current_x = xCoord + i;
      int current_y = yCoord + j;
      draw_pixel(current_x, current_y, color);
    }
  }
  // for (int y = 0; y < window_height; y++) {
  //   for (int x = 0; x < window_width; x++) {
  //     if (y >= yCoord && y <= (yCoord + height) &&
  //         (x >= xCoord && x <= (xCoord + width))) {
  //       color_buffer[(window_width * y) + x] = color;
  //     }
  //   }
  // }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)(window_width * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++)
      color_buffer[(window_width * y) + x] = color;
  }
}

void destroy_window(void) {
  free(color_buffer);  // free memory, free is opposite of malloc
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();  // opposite to SDL_Init
}
