#ifndef _OBJECT_H
#define _OBJECT_H

#include <render.h>
#include "imath.h"

#define OBJECT_SHADING FLTSHADING

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

Object* new_object(unsigned int numPoints, unsigned int nunFaces);
void update_object(Object *obj, Matrix4 *mat, Vector3 *light_p, fixed_t light_amb);

#endif

