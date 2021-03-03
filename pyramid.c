#include "object.h"

Object* new_pyramid(fixed_t size, uint8_t color) {
  Object *obj = new_object(5, 6);

  obj->obj_coords[0].x = 0;
  obj->obj_coords[0].y = size;
  obj->obj_coords[0].z = 0;

  obj->obj_coords[1].x = size;
  obj->obj_coords[1].y = -size;
  obj->obj_coords[1].z = -size;

  obj->obj_coords[2].x = -size;
  obj->obj_coords[2].y = -size;
  obj->obj_coords[2].z = -size;

  obj->obj_coords[3].x = -size;
  obj->obj_coords[3].y = -size;
  obj->obj_coords[3].z = size;

  obj->obj_coords[4].x = size;
  obj->obj_coords[4].y = -size;
  obj->obj_coords[4].z = size;

  obj->face_list[0].a = 0;
  obj->face_list[0].b = 1;
  obj->face_list[0].c = 2;
  obj->face_list[0].normal.x = FIXED_T(0);
  obj->face_list[0].normal.y = FIXED_T(0);
  obj->face_list[0].normal.z = FIXED_T(-1);
  obj->face_list[0].color = color;

  obj->face_list[1].a = 0;
  obj->face_list[1].b = 3;
  obj->face_list[1].c = 4;
  obj->face_list[1].normal.x = FIXED_T(0);
  obj->face_list[1].normal.y = FIXED_T(0);
  obj->face_list[1].normal.z = FIXED_T(1);
  obj->face_list[1].color = color;

  obj->face_list[2].a = 2;
  obj->face_list[2].b = 1;
  obj->face_list[2].c = 3;
  obj->face_list[2].normal.x = FIXED_T(0);
  obj->face_list[2].normal.y = FIXED_T(-1);
  obj->face_list[2].normal.z = FIXED_T(0);
  obj->face_list[2].color = color;

  obj->face_list[3].a = 1;
  obj->face_list[3].b = 4;
  obj->face_list[3].c = 3;
  obj->face_list[3].normal.x = FIXED_T(0);
  obj->face_list[3].normal.y = FIXED_T(-1);
  obj->face_list[3].normal.z = FIXED_T(0);
  obj->face_list[3].color = color;

  obj->face_list[4].a = 0;
  obj->face_list[4].b = 2;
  obj->face_list[4].c = 3;
  obj->face_list[4].normal.x = FIXED_T(-1);
  obj->face_list[4].normal.y = FIXED_T(0);
  obj->face_list[4].normal.z = FIXED_T(0);
  obj->face_list[4].color = color;

  obj->face_list[5].a = 0;
  obj->face_list[5].b = 4;
  obj->face_list[5].c = 1;
  obj->face_list[5].normal.x = FIXED_T(1);
  obj->face_list[5].normal.y = FIXED_T(0);
  obj->face_list[5].normal.z = FIXED_T(0);
  obj->face_list[5].color = color;

  return obj;
}

