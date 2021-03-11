#include <jagdefs.h>
#include <jagtypes.h>
#include <stdlib.h>
#include <interrupt.h>
#include <render.h>

#include "object.h"

static polygon* new_triangle(polygon *next) {
  size_t sz = sizeof(polygon) + 3 * sizeof(vertex);
  polygon *result = (polygon *)malloc(sz);
  memset(result, 0, sz);
  result->size = 3;
  result->next = next;
  result->flags = OBJECT_SHADING;

  return result;
}

Object* new_object(const unsigned int numPoints, const unsigned int numFaces) {
  Object *obj = (Object *) malloc(sizeof(Object));

  obj->numPoints = numPoints;

  obj->obj_coords = (Vector3 *) malloc(numPoints * sizeof(Vector3));
  obj->world_coords = (Vector3i *) malloc(numPoints * sizeof(Vector3i));
  obj->proj_coords = (Vector2i *) malloc(numPoints * sizeof(Vector2i));

  obj->numFaces = numFaces;
  obj->face_list = (Triangle *) malloc(numFaces * sizeof(Triangle));
  obj->ply_list = (polygon **) malloc(numFaces * sizeof(polygon*));

  unsigned int i;
  polygon *last_ply = NULL;
  for (i = 0; i < numFaces; i++) {
    last_ply = new_triangle(last_ply);
    obj->ply_list[i] = last_ply;
  }

  obj->render_list = last_ply;

  return obj;
}

void connect_to_object(Object *obj1, const Object *obj2) {
  obj1->ply_list[0]->next = obj2->render_list;
}

void set_object_flags(Object *obj, const short int flags) {
  unsigned int i;

  for (i = 0; i < obj->numFaces; i++) {
    obj->ply_list[i]->flags = flags;
  }
}

void update_object(Object *obj, Matrix4 *mat, const Vector3 *light_p, const fixed_t light_amb) {
  applyMatrix(obj->world_coords, mat, obj->obj_coords, obj->numPoints);
  applyProjection(obj->proj_coords, obj->world_coords, obj->numPoints);

  unsigned int i;
  for (i = 0; i < obj->numFaces; i++) {
    Vector3 normal;

    multiplyMatrixVector3(&normal, mat, &obj->face_list[i].normal);
    fixed_t cos_theta = dotProduct(light_p, &normal);
    if (cos_theta < 0) cos_theta = 0;
    fixed_t intensity = pMultiply(FIXED_T(255), cos_theta) + light_amb;
    if (intensity > FIXED_T(255))
      intensity = FIXED_T(255);

#define NR (-1)
    obj->ply_list[i]->vertices[0].x = obj->proj_coords[obj->face_list[i].a].x << 16;
    obj->ply_list[i]->vertices[0].y = obj->proj_coords[obj->face_list[i].a].y << 16;
    //obj->ply_list[i]->vertices[0].z = obj->world_coords[obj->face_list[i].a].z << 16;
    obj->ply_list[i]->vertices[0].z = NR << 16;

    obj->ply_list[i]->vertices[1].x = obj->proj_coords[obj->face_list[i].b].x << 16;
    obj->ply_list[i]->vertices[1].y = obj->proj_coords[obj->face_list[i].b].y << 16;
    //obj->ply_list[i]->vertices[1].z = obj->world_coords[obj->face_list[i].b].z << 16;
    obj->ply_list[i]->vertices[1].z = NR << 16;

    obj->ply_list[i]->vertices[2].x = obj->proj_coords[obj->face_list[i].c].x << 16;
    obj->ply_list[i]->vertices[2].y = obj->proj_coords[obj->face_list[i].c].y << 16;
    //obj->ply_list[i]->vertices[2].z = obj->world_coords[obj->face_list[i].c].z << 16;
    obj->ply_list[i]->vertices[2].z = NR << 16;

    obj->ply_list[i]->param = (obj->face_list[i].color << 8) | (intensity >> PSHIFT)
; 
  }

}

