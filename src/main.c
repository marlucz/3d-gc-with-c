#include "display.h"
#include "vector.h"

// declare an array of vectors/points
#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];       // 9x9x9 cube
vec2_t projected_points[N_POINTS];  // cube_points projected into 2D space

float fov_factor = 128;

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

// Function that receives 3D vector and return a projected 2D point
vec2_t project(vec3_t point) {
  vec2_t projected_point = {.x = (fov_factor * point.x),
                            .y = (fov_factor * point.y)};

  return projected_point;
}

void update(void) {
  for (int i = 0; i < N_POINTS; i++) {
    vec3_t point = cube_points[i];

    // project the current point
    vec2_t projected_point = project(point);
    // save the projected 2D vector in the array that will be used to render the
    // points
    projected_points[i] = projected_point;
  }
}

void render(void) {
  // removed because we render color and clear buffer by hand later
  // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  // SDL_RenderClear(renderer);

  draw_grid();
  // draw_rect(200, 200, 500, 200, 0xFF0000FF);
  // draw_pixel(20, 20, 0XFFFFFF00);

  // loop all projected points and render them
  for (int i = 0; i < N_POINTS; i++) {
    vec2_t projected_point = projected_points[i];
    draw_rect(projected_point.x + window_width / 2,
              projected_point.y + window_height / 2, 4, 4, 0xFFFFFF00);
  }

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