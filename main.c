#include <jagdefs.h>
#include <jagtypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <interrupt.h>
#include <sprite.h>
#include <joypad.h>
#include <screen.h>
#include <blit.h>
#include <fb2d.h>
#include <render.h>
#include <console.h>
#include <sound.h>

#include "imath.h"
#include "object.h"

#define MAXX     FIXED_T(HALFW)
#define MINX     FIXED_T(-HALFW)
#define MAXY     FIXED_T(HALFH)
#define MINY     FIXED_T(-HALFH)
#define MINZ     FIXED_T(-120)
#define MAXZ     FIXED_T(256)

#define MOVE_SPEED FLOAT_TO_FIXED(0.3)

#define SHADING FLTSHADING

#define AMBIENT_LIGHT FIXED_T(72)
#define FACE_COLOR_1 0x00
#define FACE_COLOR_2 0xf0

#define NUM_POINTS 5
#define NUM_FACES 6

#define SIZE 32

Vector3 light_source = {
  FLOAT_TO_FIXED(-0.577), FLOAT_TO_FIXED(0.577), FLOAT_TO_FIXED(-0.577)
};

Object *obj;
polygon *poly[NUM_FACES];

fixed_t xpos;
fixed_t ypos;
fixed_t zpos;
int xangle;
int yangle;
int zangle;
Matrix4 m_rot;
Matrix4 m_trans;
Matrix4 m_world;

polygon *alloc_poly(nvertices) {
  size_t sz = sizeof(polygon) + nvertices * sizeof(vertex);
  polygon *result = (polygon *)malloc(sz);
  memset(result, 0, sz);
  result->size = nvertices;
  return result;
}

void update_poly(polygon *poly, Vector2i *points, unsigned int numPoints) {
  unsigned int i;

  for (i = 0; i < numPoints; i++) {
    poly->vertices[i].x = points[i].x << 16;
    poly->vertices[i].y = points[i].y << 16;
  }
}

void update(void) {
  unsigned int i;

  if (++xangle > MAX_ANGLE) {
    xangle = 0;
  }

  mRotateX(&m_rot, xangle);
  mTranslate(&m_trans, xpos, ypos, zpos);
  mMultiply(&m_world, &m_trans, &m_rot);

  applyMatrix(obj->world_coords, &m_world, obj->obj_coords, obj->numPoints);
  applyProjection(obj->proj_coords, obj->world_coords, obj->numPoints);

  for (i = 0; i < obj->numFaces; i++) {
    Vector3 normal;

    multiplyMatrixVector3(&normal, &m_world, &obj->face_list[i].normal);
    fixed_t cos_theta = dotProduct(&light_source, &normal);
    if (cos_theta < 0) cos_theta = 0;
    fixed_t intensity = pMultiply(FIXED_T(255), cos_theta) + AMBIENT_LIGHT;
    if (intensity > FIXED_T(255))
      intensity = FIXED_T(255);

    obj->face_list[i].intensity = intensity >> PSHIFT;

    poly[i]->vertices[0].x = obj->proj_coords[obj->face_list[i].a].x << 16;
    poly[i]->vertices[0].y = obj->proj_coords[obj->face_list[i].a].y << 16;

    poly[i]->vertices[1].x = obj->proj_coords[obj->face_list[i].b].x << 16;
    poly[i]->vertices[1].y = obj->proj_coords[obj->face_list[i].b].y << 16;

    poly[i]->vertices[2].x = obj->proj_coords[obj->face_list[i].c].x << 16;
    poly[i]->vertices[2].y = obj->proj_coords[obj->face_list[i].c].y << 16;

    poly[i]->param = (obj->face_list[i].color << 8) | obj->face_list[i].intensity;
  }
}

void init(void) {
  xpos = FIXED_T(0);
  ypos = FIXED_T(0);
  zpos = FIXED_T(0);
  xangle = 0;
  yangle = 0;
  zangle = 0;

  poly[0] = alloc_poly(3);
  poly[0]->next = NULL;
  poly[0]->flags = SHADING;

  poly[1] = alloc_poly(3);
  poly[1]->next = poly[0];
  poly[1]->flags = SHADING;

  poly[2] = alloc_poly(3);
  poly[2]->next = poly[1];
  poly[2]->flags = SHADING;

  poly[3] = alloc_poly(3);
  poly[3]->next = poly[2];
  poly[3]->flags = SHADING;

  poly[4] = alloc_poly(3);
  poly[4]->next = poly[3];
  poly[4]->flags = SHADING;

  poly[5] = alloc_poly(3);
  poly[5]->next = poly[4];
  poly[5]->flags = SHADING;

  obj = new_object(NUM_POINTS, NUM_FACES);

  obj->obj_coords[0].x = FIXED_T(0);
  obj->obj_coords[0].y = FIXED_T(SIZE);
  obj->obj_coords[0].z = FIXED_T(0);

  obj->obj_coords[1].x = FIXED_T(SIZE);
  obj->obj_coords[1].y = FIXED_T(-SIZE);
  obj->obj_coords[1].z = FIXED_T(-SIZE);

  obj->obj_coords[2].x = FIXED_T(-SIZE);
  obj->obj_coords[2].y = FIXED_T(-SIZE);
  obj->obj_coords[2].z = FIXED_T(-SIZE);

  obj->obj_coords[3].x = FIXED_T(-SIZE);
  obj->obj_coords[3].y = FIXED_T(-SIZE);
  obj->obj_coords[3].z = FIXED_T(SIZE);

  obj->obj_coords[4].x = FIXED_T(SIZE);
  obj->obj_coords[4].y = FIXED_T(-SIZE);
  obj->obj_coords[4].z = FIXED_T(SIZE);

  obj->face_list[0].a = 0;
  obj->face_list[0].b = 1;
  obj->face_list[0].c = 2;
  obj->face_list[0].normal.x = FIXED_T(0);
  obj->face_list[0].normal.y = FIXED_T(0);
  obj->face_list[0].normal.z = FIXED_T(-1);
  obj->face_list[0].color = FACE_COLOR_2;
  obj->face_list[0].intensity = 0xff;

  obj->face_list[1].a = 0;
  obj->face_list[1].b = 3;
  obj->face_list[1].c = 4;
  obj->face_list[1].normal.x = FIXED_T(0);
  obj->face_list[1].normal.y = FIXED_T(0);
  obj->face_list[1].normal.z = FIXED_T(1);
  obj->face_list[1].color = FACE_COLOR_2;
  obj->face_list[1].intensity = 0xff;

  obj->face_list[2].a = 2;
  obj->face_list[2].b = 1;
  obj->face_list[2].c = 3;
  obj->face_list[2].normal.x = FIXED_T(0);
  obj->face_list[2].normal.y = FIXED_T(-1);
  obj->face_list[2].normal.z = FIXED_T(0);
  obj->face_list[2].color = FACE_COLOR_1;
  obj->face_list[2].intensity = 0xff;

  obj->face_list[3].a = 1;
  obj->face_list[3].b = 4;
  obj->face_list[3].c = 3;
  obj->face_list[3].normal.x = FIXED_T(0);
  obj->face_list[3].normal.y = FIXED_T(-1);
  obj->face_list[3].normal.z = FIXED_T(0);
  obj->face_list[3].color = FACE_COLOR_1;
  obj->face_list[3].intensity = 0xff;

  obj->face_list[4].a = 0;
  obj->face_list[4].b = 2;
  obj->face_list[4].c = 3;
  obj->face_list[4].normal.x = FIXED_T(1);
  obj->face_list[4].normal.y = FIXED_T(0);
  obj->face_list[4].normal.z = FIXED_T(0);
  obj->face_list[4].color = FACE_COLOR_1;
  obj->face_list[4].intensity = 0xff;

  obj->face_list[5].a = 0;
  obj->face_list[5].b = 4;
  obj->face_list[5].c = 1;
  obj->face_list[4].normal.x = FIXED_T(-1);
  obj->face_list[4].normal.y = FIXED_T(0);
  obj->face_list[4].normal.z = FIXED_T(0);
  obj->face_list[5].color = FACE_COLOR_1;
  obj->face_list[5].intensity = 0xff;

  update();
}

int main(int argc, char *argv[]) {
  joypad_state *j_state = malloc(sizeof(joypad_state));

  TOMREGS->vmode = CRY16|CSYNC|BGEN|PWIDTH4|VIDEN;
  init_interrupts();

  init_display_driver();

  display *d = new_display(0);

  d->x = 32;
  d->y = 16;

  screen *frame1 = new_screen();
  screen *frame2 = new_screen();
  phrase *frame_data = alloc_z_double_buffered_screens(DEPTH16,320,240,frame1,frame2);
  sprite *frame_sprite = sprite_of_screen(0,0,frame1);

  frame_sprite->trans = 1;

  void *renderer_addr = &_GPU_FREE_RAM;
  void *gpu_available_ram = init_renderer(renderer_addr);

  show_display(d);

  attach_sprite_to_display_at_layer(frame_sprite,d,1);

  screen *phys = frame1;
  screen *log = frame2;
  screen *tmp = phys;

  clear_screen(frame1);

  init_imath();
  init();
  for(;;) {
    vsync();
    render_polygon_list(phys, poly[5], CLR_SCREEN);
    update();
    read_joypad_state(j_state);
    wait_renderer_completion();

    tmp = phys;
    phys = log;
    log = tmp;

    wait_display_refresh();

    frame_sprite->data = phys->data;

    if(j_state->j1 & JOYPAD_RIGHT) {
      xpos += MOVE_SPEED;
      if (xpos > MAXX) {
        xpos = MAXX;
      }
    } else if(j_state->j1 & JOYPAD_LEFT) {
      xpos -= MOVE_SPEED;
      if (xpos < MINX) {
        xpos = MINX;
      }
    }
    if(j_state->j1 & JOYPAD_UP) {
      ypos += MOVE_SPEED;
      if (ypos > MAXY) {
        ypos = MAXY;
      }
    } else if(j_state->j1 & JOYPAD_DOWN) {
      ypos -= MOVE_SPEED;
      if (ypos < MINY) {
        ypos = MINY;
      }
    }
    if(j_state->j1 & JOYPAD_C) {
      zpos += MOVE_SPEED;
      if (zpos > MAXZ) {
        zpos = MAXZ;
      }
    } else if(j_state->j1 & JOYPAD_A) {
      zpos -= MOVE_SPEED;
      if (zpos < MINZ) {
        zpos = MINZ;
      }
    }
  }

  return 0;
}

