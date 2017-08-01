#ifndef VEC3_MATH_HPP
#define VEC3_MATH_HPP

/*
 * Essential parts copied over from gl-matrix.c library.
 * Please see https://github.com/coreh/gl-matrix.c
 *
 * gl-matrix.c
 * C Matrix library for OpenGL apps. A port of the gl-matrix JavaScript library.
 *
 * Copyright (c) 2011 Brandon Jones
 */

typedef double *vec3_t;

/*
 * vec3_t - 3 Dimensional Vector
 */

/*
 * vec3_create
 * Creates a new instance of a vec3_t
 *
 * Params:
 * vec - Optional, vec3_t containing values to initialize with. If NULL, the
 * vector will be initialized with zeroes.
 *
 * Returns:
 * New vec3
 */
vec3_t vec3_create(vec3_t vec);

/*
 * vec3_delete
 * Frees memory used by a vec3_t instance
 *
 * Params:
 * vec - vec3_t that needs to be freed
 *
 * Returns:
 * void
 */
void vec3_delete(vec3_t vec);

/*
 * vec3_set
 * Copies the values of one vec3_t to another
 *
 * Params:
 * vec - vec3_t containing values to copy
 * dest - vec3_t receiving copied values
 *
 * Returns:
 * dest
 */
vec3_t vec3_set(vec3_t vec, vec3_t dest);

/*
 * vec3_add
 * Performs a vector addition
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_add(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_subtract
 * Performs a vector subtraction
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_subtract(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_multiply
 * Performs a vector multiplication
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_multiply(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_negate
 * Negates the components of a vec3
 *
 * Params:
 * vec - vec3_t to negate
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_negate(vec3_t vec, vec3_t dest);

/*
 * vec3_scale
 * Multiplies the components of a vec3_t by a scalar value
 *
 * Params:
 * vec - vec3_t to scale
 * val - Numeric value to scale by
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_scale(vec3_t vec, double val, vec3_t dest);

/*
 * vec3_normalize
 * Generates a unit vector of the same direction as the provided vec3
 * If vector length is 0, returns [0, 0, 0]
 *
 * Params:
 * vec - vec3_t to normalize
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_normalize(vec3_t vec, vec3_t dest);

/*
 * vec3_cross
 * Generates the cross product of two vec3s
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_cross (vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_length
 * Caclulates the length of a vec3
 *
 * Params:
 * vec - vec3_t to calculate length of
 *
 * Returns:
 * Length of vec
 */
double vec3_length(vec3_t vec);

/*
 * vec3_dot
 * Caclulates the dot product of two vec3s
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 *
 * Returns:
 * Dot product of vec and vec2
 */
double vec3_dot(vec3_t vec, vec3_t vec2);

#endif
