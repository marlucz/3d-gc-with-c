#include "display.h"
#include "vector.h"

// declare an array of vectors/points
const int M_POINTS = 9 * 9 * 0;
vec3_t cube_points[M_POINTS];  // 9x9x9 cube

bool is_running = false;

void setup(void) {
  // allocate the required memory in bytes to hold the color buffer
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  // creating an SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  // start loading my array of vectors
  // from -1 to 1 (in this 9x9x9 cube)
  int point_count = 0;
  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {
            .x = x, .y = y, .z = z};  // for the specific field .x assign value
        cube_points[point_count++] = new_point;
      }
    }
  }
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

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid();
  draw_rect(200, 200, 500, 200, 0xFF0000FF);
  draw_pixel(20, 20, 0XFFFFFF00);
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
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