#include "array.h"
#include "display.h"
#include "mesh.h"
#include "vector.h"

// pointer in the memory for the array of triangles that should be rendered
triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
  // allocate the required memory in bytes to hold the color buffer
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  // creating an SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  load_cube_mesh_data();
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event); // & - means a reference to the event

  switch (event.type) {
  case SDL_QUIT: // SDL_QUIT - user clicks on the close button
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = false;
    break;
  }
}

// Function that receives 3D vector and return a projected 2D point
vec2_t project(vec3_t point) {
  vec2_t projected_point = {.x = (fov_factor * point.x) / point.z,
                            .y = (fov_factor * point.y) / point.z};

  return projected_point;
}

void update(void) {
  // lock the update execution unless we hit frame target time since last frame
  // DO NOT USE WHILE LOOPS FOR THAT - IT BLOCKS 100% CPU USAGE
  // while (!SDL_TICKS_PASSED(SDL_GetTicks(),
  //                          previous_frame_time + FRAME_TARGET_TIME))
  //   ;

  // do the SDL_Delay instead

  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // initialize the array of triangles to render
  // reset on every loop
  triangles_to_render = NULL;

  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.01;
  mesh.rotation.z += 0.01;

  int num_faces = array_length(mesh.faces);

  // Loop all triangle faces of our mesh
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] =
        mesh.vertices[mesh_face.a - 1]; // -1 as a compensation for vertex
                                        // indexing in the mesh_faces
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    // Loop all three vertices of this current face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      // Translate the vertex away from camera in z
      transformed_vertex.z += camera_position.z;

      // Project the current vertex
      vec2_t projected_point = project(transformed_vertex);

      // Scale and translate the projected points to the middle of the screen
      projected_point.x += window_width / 2,
          projected_point.y += window_height / 2,

          projected_triangle.points[j] = projected_point;
    }

    // Save the projected triangle in the array of triangles to render
    // triangles_to_render[i] = projected_triangle;
    array_push(triangles_to_render, projected_triangle);
  }
}

void render(void) {
  // removed because we render color and clear buffer by hand later
  // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  // SDL_RenderClear(renderer);

  draw_grid();
  // draw_rect(200, 200, 500, 200, 0xFF0000FF);
  // draw_pixel(20, 20, 0XFFFFFF00);

  // Loop all projected triangles and render them
  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    // Draw vertex points
    draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
    draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

    // Draw triangle with lines
    draw_triangle(triangle.points[0].x, triangle.points[0].y,
                  triangle.points[1].x, triangle.points[1].y,
                  triangle.points[2].x, triangle.points[2].y, 0xFF00FF00);
  }

  // clear the array of triangles to render every frame loop
  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

// Free the memory that was dynamically allocated by the program
void free_resources(void) {
  free(color_buffer); // free memory, free is opposite of malloc
  array_free(mesh.faces);
  array_free(mesh.vertices);
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
  free_resources();

  return 0;
}