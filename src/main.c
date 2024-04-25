#include <math.h>

#include "array.h"
#include "display.h"
#include "mesh.h"
#include "vector.h"

// pointer in the memory for the array of triangles that should be rendered
triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, 0};

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

  // Loads the cube values in the mesh data structure
  load_cube_mesh_data();
  // load_obj_file_data("./assets/cube.obj");
}

void handle_key_press(SDL_Keycode keycode) {
  switch (keycode) {
    case SDLK_ESCAPE:
      is_running = false;
      break;
    case SDLK_1:
      CULL_BACKFACE = !CULL_BACKFACE;
      break;
    case SDLK_2:
      RENDER_WIREFRAME = !RENDER_WIREFRAME;
      break;
    case SDLK_3:
      RENDER_FILL = !RENDER_FILL;
      break;
    case SDLK_4:
      RENDER_VERTICES = !RENDER_VERTICES;
      break;
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
      handle_key_press(event.key.keysym.sym);
      break;
  }
}

// Function that receives 3D vector and return a projected 2D point
// Convert 3D point to 2D
vec2_t project(vec3_t point) {
  float w = 1;

  float x_2 = point.x / point.z * w;

  float rb = sqrt(pow(x_2, 2.0) + pow(w, 2.0));
  float cb = (rb * point.z) / w;

  float y_2 = (point.y * rb) / cb;

  vec2_t projected_point = {.x = fov_factor * (x_2), .y = fov_factor * (y_2)};

  // Old solution (simplified one, works if w = 1)
  // vec2_t projected_point = {
  //   .x = fov_factor * (point.x) / point.z,
  //   .y = fov_factor * (point.y) / point.z
  // };

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
        mesh.vertices[mesh_face.a - 1];  // -1 as a compensation for vertex
                                         // indexing in the mesh_faces
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    vec3_t transformed_vertices[3];

    // Loop all three vertices of this current face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      // Translate the vertex away from camera in z
      transformed_vertex.z += -5;

      // save transformed vertex in the array of transformed vertices;
      transformed_vertices[j] = transformed_vertex;
    }

    if (CULL_BACKFACE) {
      // Backface culling check
      vec3_t vector_a = transformed_vertices[0]; /*  A */
      vec3_t vector_b = transformed_vertices[1]; /* /\ */
      vec3_t vector_c = transformed_vertices[2]; /*C--B*/

      // Get the vector subtraction of B-A and C-A
      vec3_t vector_ab = vec3_sub(vector_b, vector_a);
      vec3_normalize(&vector_ab);
      vec3_t vector_ac = vec3_sub(vector_c, vector_a);
      vec3_normalize(&vector_ac);

      // Compute the face normal (using cross product to find perpendicular)
      vec3_t normal = vec3_cross(vector_ab, vector_ac);
      // Normalize the face normal vector
      vec3_normalize(
          &normal);  // & means we're passing a memory reference to that value
                     // so every operation is done directly on the ref

      // Find the vector between a point in the triangle and the camera origin
      vec3_t camera_ray = vec3_sub(camera_position, vector_a);

      // Calculate how aligned the camera ray is with the face normal (using dot
      // product)
      float dot_normal_camera = vec3_dot(camera_ray, normal);

      // bypass the triangles that are looking away from the camera
      if (dot_normal_camera < 0) {
        continue;
      }
    }

    vec2_t projected_points[3];

    // loop all three vertices to perform projection
    for (int j = 0; j < 3; j++) {
      // Project the current vertex
      projected_points[j] = project(transformed_vertices[j]);

      // Scale and translate the projected points to the middle of the screen
      projected_points[j].x += window_width / 2;
      projected_points[j].y += window_height / 2;
    }

    triangle_t projected_triangle = {
        .points =
            {
                {projected_points[0].x, projected_points[0].y},
                {projected_points[1].x, projected_points[1].y},
                {projected_points[2].x, projected_points[2].y},
            },
        .color = mesh_face.color};

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

    if (RENDER_VERTICES) {
      float vw = 6.0;  // vertex width
      // Draw vertex points
      draw_rect(triangle.points[0].x - vw / 2, triangle.points[0].y - vw / 2,
                vw, vw, 0xFFFFFF00);
      draw_rect(triangle.points[1].x - vw / 2, triangle.points[1].y - vw / 2,
                vw, vw, 0xFFFFFF00);
      draw_rect(triangle.points[2].x - vw / 2, triangle.points[2].y - vw / 2,
                vw, vw, 0xFFFFFF00);
    }

    if (RENDER_FILL) {
      // Draw filled triangle
      draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                           triangle.points[1].x, triangle.points[1].y,
                           triangle.points[2].x, triangle.points[2].y,
                           triangle.color);
    }
    if (RENDER_WIREFRAME) {
      // Draw unfilled triangle
      draw_triangle(triangle.points[0].x, triangle.points[0].y,
                    triangle.points[1].x, triangle.points[1].y,
                    triangle.points[2].x, triangle.points[2].y, 0xFF000000);
    }
  }

  // clear the array of triangles to render every frame loop
  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

// Free the memory that was dynamically allocated by the program
void free_resources(void) {
  free(color_buffer);  // free memory, free is opposite of malloc
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