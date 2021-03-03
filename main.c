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
#include "pyramid.h"

#define ROOT_OBJECT 0
#define OBJECT_SIZE  FLOAT_TO_FIXED(32.0)

#define AMBIENT_LIGHT FIXED_T(72)
#define FACE_COLOR_1 0x00
#define FACE_COLOR_2 0xf0

#define MAXX     FIXED_T(HALFW)
#define MINX     FIXED_T(-HALFW)
#define MAXY     FIXED_T(HALFH)
#define MINY     FIXED_T(-HALFH)
#define MINZ     FIXED_T(-120)
#define MAXZ     FIXED_T(256)

#define MOVE_SPEED FLOAT_TO_FIXED(0.3)

#define MODE_NO_SORT     0
#define MODE_Z_BUFFER    1

const Vector3 light_source = {
  FLOAT_TO_FIXED(-0.577), FLOAT_TO_FIXED(0.577), FLOAT_TO_FIXED(-0.577)
};

Object *obj[2];

unsigned int lock_keys;
fixed_t xpos;
fixed_t ypos;
fixed_t zpos;
int xangle;
int yangle;
int zangle;
Matrix4 m_rot;
Matrix4 m_trans;
Matrix4 m_world;

int sort_mode;

void update(void) {
  if (++xangle > MAX_ANGLE) {
    xangle = 0;
  }

  mRotateX(&m_rot, xangle);
  mTranslate(&m_trans, xpos + OBJECT_SIZE, ypos, zpos);
  mMultiply(&m_world, &m_trans, &m_rot);

  update_object(obj[0], &m_world, &light_source, AMBIENT_LIGHT);

  mRotateX(&m_rot, xangle);
  mTranslate(&m_trans, xpos - OBJECT_SIZE, ypos, zpos);
  mMultiply(&m_world, &m_trans, &m_rot);

  update_object(obj[1], &m_world, &light_source, AMBIENT_LIGHT);
}

void init(void) {
  xpos = FIXED_T(0);
  ypos = FIXED_T(0);
  zpos = FIXED_T(0);
  xangle = 0;
  yangle = 0;
  zangle = 0;

  obj[0] = new_pyramid(OBJECT_SIZE, FACE_COLOR_1);
  obj[1] = new_pyramid(OBJECT_SIZE, FACE_COLOR_2);
  connect_to_object(obj[ROOT_OBJECT], obj[1]);

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
  lock_keys = 0;
  sort_mode = MODE_NO_SORT;

  for(;;) {
    vsync();
    if (sort_mode == MODE_NO_SORT) {
      render_polygon_list(phys, obj[ROOT_OBJECT]->render_list, CLR_SCREEN);
    }
    else if (sort_mode == MODE_Z_BUFFER) {
      render_polygon_list(phys, obj[ROOT_OBJECT]->render_list, CLR_SCREEN | CLR_Z_SCREEN);
    }
    update();
    read_joypad_state(j_state);
    wait_renderer_completion();

    tmp = phys;
    phys = log;
    log = tmp;

    wait_display_refresh();

    frame_sprite->data = phys->data;

    if (lock_keys) {
      if ((j_state->j1 & JOYPAD_1) == 0) {
        lock_keys &= ~JOYPAD_1;
      }
    }

    if (j_state->j1 & JOYPAD_1) {
      if ((lock_keys & JOYPAD_1) == 0) {
        if (sort_mode == MODE_NO_SORT) {
          set_object_flags(obj[0], FLTSHADING | ZBUFFERING);
          set_object_flags(obj[1], FLTSHADING | ZBUFFERING);
          sort_mode = MODE_Z_BUFFER;
        }
        else if (sort_mode == MODE_Z_BUFFER) {
          set_object_flags(obj[0], FLTSHADING);
          set_object_flags(obj[1], FLTSHADING);
          sort_mode = MODE_NO_SORT;
        }
        lock_keys |= JOYPAD_1;
      }
    }

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

