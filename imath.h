#ifndef _IMATH_H
#define _IMATH_H

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

#define MAX_ANGLE	255
#define MIN_SCALE	3

#define PRES            256
#define PSHIFT          8
#define PROUNDBIT       (1 << (PSHIFT-1))

#define HALFW           160
#define HALFH           120
#define FOV             128

#include <stdint.h>
#include <render.h>

typedef int32_t fixed_t;

typedef struct {
  fixed_t x, y, z;
} Vector3;

typedef struct {
  int x, y, z;
} Vector3i;

typedef struct {
  fixed_t m[4][4];
} Matrix4;

extern fixed_t sintab[MAX_ANGLE + 1];
extern fixed_t costab[MAX_ANGLE + 1];

#define FIXED_T(x) (x << PSHIFT)
#define FLOAT_TO_FIXED(f) ((f) * PRES)

#define SIN(a) sintab[(a)]
#define COS(a) costab[(a)]

void init_imath(void);

void mIdentity(Matrix4 *mat);
void mMultiply(Matrix4 *mat, const Matrix4 *mat1, const Matrix4 *mat2);
void mRotateX(Matrix4 *mat, const unsigned int angle);
void mRotateY(Matrix4 *mat, const unsigned int angle);
void mRotateZ(Matrix4 *mat, const unsigned int angle);
void mTranslate(Matrix4 *mat, const fixed_t x, const fixed_t y, const fixed_t z);
void mScale(Matrix4 *mat, const float ratio);
void applyMatrix(Vector3i *result, const Matrix4 *mat, const Vector3 *points, const unsigned int numPoints);
void applyProjection(vertex *proj, const Vector3i *points, const unsigned int numPoints);

#endif

