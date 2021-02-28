#include <stdlib.h>
#include "object.h"

Object* new_object(unsigned int numPoints, unsigned int numFaces) {
  Object *obj = (Object *) malloc(sizeof(Object));

  obj->numPoints = numPoints;

  obj->obj_coords = (Vector3 *) malloc(numPoints * sizeof(Vector3));
  obj->world_coords = (Vector3i *) malloc(numPoints * sizeof(Vector3i));
  obj->proj_coords = (Vector2i *) malloc(numPoints * sizeof(Vector2i));

  obj->numFaces = numFaces;
  obj->face_list = (Triangle *) malloc(numFaces * sizeof(Triangle));

  return obj;
}

