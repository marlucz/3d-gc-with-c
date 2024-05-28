#include "matrix.h"

mat4_t mat4_identity(void) {
  // | 1 0 0 0 |
  // | 0 1 0 0 |
  // | 0 0 1 0 |
  // | 0 0 0 1 |
  mat4_t m = {{
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
  }};
  return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
  // | sx 0  0  0 |   | x |   | x * sx |
  // | 0  sy 0  0 | X | y | = | y * sy |
  // | 0  0  sz 0 |   | z |   | z * sz |
  // | 0  0  0  1 |   | 1 |   | 1 |

  mat4_t m = mat4_identity();
  m.m[0][0] = sx;
  m.m[1][1] = sy;
  m.m[2][2] = sz;
  return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
  // | 1 0 0 tx |   | x |   | x + tx |
  // | 0 1 0 ty | X | y | = | y + ty |
  // | 0 0 1 tz |   | z |   | z + tz |
  // | 0 0 0 1  |   | 1 |   | 1 |

  mat4_t m = mat4_identity();
  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;
  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
  vec4_t result;
  result.x =
      m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y =
      m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z =
      m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w =
      m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}