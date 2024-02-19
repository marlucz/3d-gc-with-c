#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

typedef struct {
    int a;
    int b;
    int c;
} face_t;

// store projected points of the face
typedef struct {
    vec2_t points[3];
} triangle_t;

#endif