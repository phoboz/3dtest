#ifndef _OBJECT_H
#define _OBJECT_H

#include "imath.h"

typedef struct {
  unsigned int a, b, c;

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
} Object;

Object* new_object(unsigned int numPoints, unsigned int nunFaces);

#endif

