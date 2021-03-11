#ifndef _OBJECT_H
#define _OBJECT_H

#include <render.h>
#include "imath.h"

#define OBJECT_MIN_Z   128

#define OBJECT_DEFAULT_FLAGS FLTSHADING | ZBUFFERING

typedef struct {
  unsigned int a, b, c;
  Vector3 normal;

  uint8_t color;
  uint8_t intensity;
} Triangle;

typedef struct {
  unsigned int numPoints;

  Vector3 *obj_coords;
  Vector3i *world_coords;
  Vector2i *proj_coords;

  unsigned int numFaces;
  Triangle *face_list;
  polygon **ply_list;

  polygon *render_list;
} Object;

Object* new_object(const unsigned int numPoints, const unsigned int nunFaces);
void connect_to_object(Object *obj1, const Object *obj2);
void set_object_flags(Object *obj, const short int flags);
void update_object(Object *obj, Matrix4 *mat, const Vector3 *light_p, const fixed_t light_amb);

#endif

