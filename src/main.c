#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool is_running = false;

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

void setup(void) {
  // allocate the required memory in bytes to hold the color buffer
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  // creating an SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);  // & - means a reference to the event

  switch (event.type) {
    case SDL_QUIT:  // SDL_QUIT - user clicks on the close button
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) is_running = false;
      break;
  }
}

void update(void) {}

void draw_grid() {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      if (y % 10 == 0 || x % 10 == 0) {
        color_buffer[(window_width * y) + x] = 0xFF333333;
      }
    }
  };
}

void draw_rect(int xCoord, int yCoord, int width, int height, uint32_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      if (y >= yCoord && y <= (yCoord + height) &&
          (x >= xCoord && x <= (xCoord + width))) {
        color_buffer[(window_width * y) + x] = color;
      }
    }
  }
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

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid();
  draw_rect(200, 200, 500, 200, 0xFF0000FF);
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

void destroy_window(void) {
  free(color_buffer);  // free memory, free is opposite of malloc
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();  // opposite to SDL_Init
}

int main(int argc, char *argv[]) {
  is_running = initialize_window();

  setup();

  // game loop
  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();

  return 0;
}