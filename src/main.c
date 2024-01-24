#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

bool is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }
  // Create SDL Window
  window = SDL_CreateWindow(NULL,                    // Window title
                            SDL_WINDOWPOS_CENTERED,  // x position
                            SDL_WINDOWPOS_CENTERED,  // y position
                            800,                     // Window width
                            600,                     // Window height
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

  return true;
}

int main(void) {
  is_running = initialize_window();

  return 0;
}