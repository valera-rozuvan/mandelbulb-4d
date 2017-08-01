#include <stdlib.h>
#include <math.h>
#include "vec3_math.hpp"

/*
 * Essential parts copied over from gl-matrix.c library.
 * Please see https://github.com/coreh/gl-matrix.c
 *
 * gl-matrix.c
 * C Matrix library for OpenGL apps. A port of the gl-matrix JavaScript library.
 *
 * Copyright (c) 2011 Brandon Jones
 */

vec3_t vec3_create(vec3_t vec) {
  vec3_t dest = (vec3_t)calloc(sizeof(double_t), 3);

  if (vec) {
    dest[0] = vec[0];
    dest[1] = vec[1];
    dest[2] = vec[2];
  } else {
    dest[0] = dest[1] = dest[2] = 0;
  }

  return dest;
}

void vec3_delete(vec3_t vec) {
  free(vec);
}

vec3_t vec3_set(vec3_t vec, vec3_t dest) {
  dest[0] = vec[0];
  dest[1] = vec[1];
  dest[2] = vec[2];

  return dest;
}

vec3_t vec3_add(vec3_t vec, vec3_t vec2, vec3_t dest) {
  if (!dest || vec == dest) {
    vec[0] += vec2[0];
    vec[1] += vec2[1];
    vec[2] += vec2[2];
    return vec;
  }

  dest[0] = vec[0] + vec2[0];
  dest[1] = vec[1] + vec2[1];
  dest[2] = vec[2] + vec2[2];

  return dest;
}

vec3_t vec3_subtract(vec3_t vec, vec3_t vec2, vec3_t dest) {
  if (!dest || vec == dest) {
    vec[0] -= vec2[0];
    vec[1] -= vec2[1];
    vec[2] -= vec2[2];
    return vec;
  }

  dest[0] = vec[0] - vec2[0];
  dest[1] = vec[1] - vec2[1];
  dest[2] = vec[2] - vec2[2];
  return dest;
}

vec3_t vec3_multiply(vec3_t vec, vec3_t vec2, vec3_t dest) {
  if (!dest || vec == dest) {
    vec[0] *= vec2[0];
    vec[1] *= vec2[1];
    vec[2] *= vec2[2];
    return vec;
  }

  dest[0] = vec[0] * vec2[0];
  dest[1] = vec[1] * vec2[1];
  dest[2] = vec[2] * vec2[2];
  return dest;
}

vec3_t vec3_negate(vec3_t vec, vec3_t dest) {
  if (!dest) { dest = vec; }

  dest[0] = -vec[0];
  dest[1] = -vec[1];
  dest[2] = -vec[2];
  return dest;
}

vec3_t vec3_scale(vec3_t vec, double val, vec3_t dest) {
  if (!dest || vec == dest) {
    vec[0] *= val;
    vec[1] *= val;
    vec[2] *= val;
    return vec;
  }

  dest[0] = vec[0] * val;
  dest[1] = vec[1] * val;
  dest[2] = vec[2] * val;
  return dest;
}

vec3_t vec3_normalize(vec3_t vec, vec3_t dest) {
  if (!dest) { dest = vec; }

  double x = vec[0], y = vec[1], z = vec[2],
    len = sqrt(x * x + y * y + z * z);

  if (!len) {
    dest[0] = 0;
    dest[1] = 0;
    dest[2] = 0;
    return dest;
  } else if (len == 1) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
    return dest;
  }

  len = 1 / len;
  dest[0] = x * len;
  dest[1] = y * len;
  dest[2] = z * len;
  return dest;
}

vec3_t vec3_cross(vec3_t vec, vec3_t vec2, vec3_t dest) {
  if (!dest) { dest = vec; }

  double x = vec[0], y = vec[1], z = vec[2],
    x2 = vec2[0], y2 = vec2[1], z2 = vec2[2];

  dest[0] = y * z2 - z * y2;
  dest[1] = z * x2 - x * z2;
  dest[2] = x * y2 - y * x2;
  return dest;
}

double vec3_length(vec3_t vec) {
  double x = vec[0], y = vec[1], z = vec[2];
  return sqrt(x * x + y * y + z * z);
}

double vec3_dot(vec3_t vec, vec3_t vec2) {
  return vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2];
}
