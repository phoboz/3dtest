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

#define MAXX     FIXED_T(HALFW)
#define MINX     -FIXED_T(HALFW)
#define MAXY     FIXED_T(HALFH)
#define MINY     -FIXED_T(HALFH)
#define MINZ     -FIXED_T(120)
#define MAXZ     FIXED_T(120)

#define MOVE_SPEED FLOAT_TO_FIXED(0.3)

#define SHADING FLTSHADING
#define MAX_NODES 3

#define SIZE 4

const Vector3 vlist1[MAX_NODES] = {
  { FIXED_T(0), FIXED_T(SIZE), FIXED_T(0) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) }
};

const Vector3 vlist2[MAX_NODES] = {
  { FIXED_T(0), FIXED_T(SIZE), FIXED_T(0) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) }
};

const Vector3 vlist3[MAX_NODES] = {
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) }
};

const Vector3 vlist4[MAX_NODES] = {
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) }
};

const Vector3 vlist5[MAX_NODES] = {
  { FIXED_T(0), FIXED_T(SIZE), FIXED_T(0) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) },
  { FIXED_T(-SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) }
};

const Vector3 vlist6[MAX_NODES] = {
  { FIXED_T(0), FIXED_T(SIZE), FIXED_T(0) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(SIZE) },
  { FIXED_T(SIZE), FIXED_T(-SIZE), FIXED_T(-SIZE) }
};

Vector3i trans_nodes[MAX_NODES];
Vector2i proj_nodes[MAX_NODES];

fixed_t xpos;
fixed_t ypos;
fixed_t zpos;
int xangle;
int yangle;
int zangle;
Matrix4 m_rot;
Matrix4 m_trans;
Matrix4 m_world;
int light;
polygon *poly1, *poly2, *poly3, *poly4, *poly5, *poly6;

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
  if (++xangle > MAX_ANGLE) {
    xangle = 0;
  }

  mRotateX(&m_rot, xangle);
  mTranslate(&m_trans, xpos, ypos, zpos);
  mMultiply(&m_world, &m_trans, &m_rot);

  applyMatrix(trans_nodes, &m_world, vlist1, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly1, proj_nodes, MAX_NODES);

  applyMatrix(trans_nodes, &m_world, vlist2, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly2, proj_nodes, MAX_NODES);

  applyMatrix(trans_nodes, &m_world, vlist3, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly3, proj_nodes, MAX_NODES);

  applyMatrix(trans_nodes, &m_world, vlist4, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly4, proj_nodes, MAX_NODES);

  applyMatrix(trans_nodes, &m_world, vlist5, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly5, proj_nodes, MAX_NODES);

  applyMatrix(trans_nodes, &m_world, vlist6, MAX_NODES);
  applyProjection(proj_nodes, trans_nodes, MAX_NODES);
  update_poly(poly6, proj_nodes, MAX_NODES);
}

void init(void) {
  xpos = FIXED_T(0);
  ypos = FIXED_T(0);
  zpos = FIXED_T(12);
  xangle = 0;
  yangle = 0;
  zangle = 0;

  light = 80 << 16;

  poly1 = alloc_poly(MAX_NODES);
  poly1->next = NULL;
  poly1->flags = SHADING;
  poly1->param = 0x88ff;

  poly1->vertices[0].i = light + (128<<16);
  poly1->vertices[1].i = light + (256<<16);
  poly1->vertices[2].i = light;

  poly2 = alloc_poly(MAX_NODES);
  poly2->next = poly1;
  poly2->flags = SHADING;
  poly2->param = 0x88ff;

  poly2->vertices[0].i = light + (128<<16);
  poly2->vertices[1].i = light + (256<<16);
  poly2->vertices[2].i = light;

  poly3 = alloc_poly(MAX_NODES);
  poly3->next = poly2;
  poly3->flags = SHADING;
  poly3->param = 0x88ff;

  poly3->vertices[0].i = light + (128<<16);
  poly3->vertices[1].i = light + (256<<16);
  poly3->vertices[2].i = light;

  poly4 = alloc_poly(MAX_NODES);
  poly4->next = poly3;
  poly4->flags = SHADING;
  poly4->param = 0x88ff;

  poly4->vertices[0].i = light + (128<<16);
  poly4->vertices[1].i = light + (256<<16);
  poly4->vertices[2].i = light;

  poly5 = alloc_poly(MAX_NODES);
  poly5->next = poly4;
  poly5->flags = SHADING;
  poly5->param = 0x88ff;

  poly5->vertices[0].i = light + (128<<16);
  poly5->vertices[1].i = light + (256<<16);
  poly5->vertices[2].i = light;

  poly6 = alloc_poly(MAX_NODES);
  poly6->next = poly5;
  poly6->flags = SHADING;
  poly6->param = 0x88ff;

  poly6->vertices[0].i = light + (128<<16);
  poly6->vertices[1].i = light + (256<<16);
  poly6->vertices[2].i = light;

  update();
}

void draw(screen *dst) {
  render_polygon_list(dst, &poly6, CLR_SCREEN);
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
    draw(phys);
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

