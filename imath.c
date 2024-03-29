#include <stdlib.h>
#include <string.h>
#include "imath.h"

extern fixed_t sintab_def[MAX_ANGLE + 1];
extern fixed_t costab_def[MAX_ANGLE + 1];

fixed_t sintab[MAX_ANGLE + 1];
fixed_t costab[MAX_ANGLE + 1];

void init_imath(void) {
  memcpy(sintab, sintab_def, sizeof(sintab_def));
  memcpy(costab, costab_def, sizeof(costab_def));
}

fixed_t dotProduct(const Vector3 *a, const Vector3 *b) {
  fixed_t result = pMultiply(a->x, b->x) +
                   pMultiply(a->y, b->y) +
                   pMultiply(a->z, b->z);

  return result;
}

void crossProduct(Vector3 *result, const Vector3 *a, const Vector3 *b) {
  result->x = pMultiply(a->y, b->z) - pMultiply(b->y,a->z);
  result->y = pMultiply(b->x, a->z) - pMultiply(a->x, b->z);
  result->z = pMultiply(a->x, b->y) - pMultiply(b->x, a->y);
}

void mIdentity(Matrix4 *mat) {
  mat->m[0][0] = PRES;
  mat->m[0][1] = 0;
  mat->m[0][2] = 0;
  mat->m[0][3] = 0;
    
  mat->m[1][0] = 0;
  mat->m[1][1] = PRES;
  mat->m[1][2] = 0;
  mat->m[1][3] = 0;
    
  mat->m[2][0] = 0;
  mat->m[2][1] = 0;
  mat->m[2][2] = PRES;
  mat->m[2][3] = 0;

  mat->m[3][0] = 0;
  mat->m[3][1] = 0;
  mat->m[3][2] = 0;
  mat->m[3][3] = PRES;
}

void mMultiply(Matrix4 *mat, const Matrix4 *mat1, const Matrix4 *mat2) {
  unsigned char r,c;
  for (c=0; c<4; c++)
    for (r=0; r<4; r++)
      mat->m[c][r] = pMultiply(mat1->m[0][r], mat2->m[c][0]) +
                     pMultiply(mat1->m[1][r], mat2->m[c][1]) +
                     pMultiply(mat1->m[2][r], mat2->m[c][2]) +
                     pMultiply(mat1->m[3][r], mat2->m[c][3]);
}

void mRotateX(Matrix4 *mat, const unsigned int angle) {
  mIdentity(mat);
  mat->m[1][1] =  COS(angle);
  mat->m[1][2] =  SIN(angle);
  mat->m[2][1] = -SIN(angle);
  mat->m[2][2] =  COS(angle);
}

void mRotateY(Matrix4 *mat, const unsigned int angle) {
  mIdentity(mat);
  mat->m[0][0] =  COS(angle);
  mat->m[0][2] = -SIN(angle);
  mat->m[2][0] =  SIN(angle);
  mat->m[2][2] =  COS(angle);
}

void mRotateZ(Matrix4 *mat, const unsigned int angle) {
  mIdentity(mat);
  mat->m[0][0] =  COS(angle);
  mat->m[0][1] =  SIN(angle);
  mat->m[1][0] = -SIN(angle);
  mat->m[1][1] =  COS(angle);
}

void mTranslate(Matrix4 *mat, const fixed_t x, const fixed_t y, const fixed_t z) {
  mIdentity(mat);
  mat->m[3][0] =  x;
  mat->m[3][1] =  y;
  mat->m[3][2] =  z;
}

void mScale(Matrix4 *mat, const float ratio) {
  mIdentity(mat);
  mat->m[0][0] *= ratio;
  mat->m[1][1] *= ratio;
  mat->m[2][2] *= ratio;
}

void multiplyMatrixVector3(Vector3 *result, const Matrix4 *mat, const Vector3 *a) {
  result->x = pMultiply(mat->m[0][0], a->x) +
              pMultiply(mat->m[1][0], a->y) +
              pMultiply(mat->m[2][0], a->z);

  result->y = pMultiply(mat->m[0][1], a->x) +
              pMultiply(mat->m[1][1], a->y) +
              pMultiply(mat->m[2][1], a->z);

  result->z = pMultiply(mat->m[0][2], a->x) +
              pMultiply(mat->m[1][2], a->y) +
              pMultiply(mat->m[2][2], a->z);
}

void applyMatrix(Vector3i *result, const Matrix4 *mat, const Vector3 *points, const unsigned int numPoints) {
  unsigned int index;

  for (index = 0; index < numPoints; index++) {
    result[index].x = (mat->m[0][0] * (points[index].x >> PSHIFT) +
                       mat->m[1][0] * (points[index].y >> PSHIFT) +
                       mat->m[2][0] * (points[index].z >> PSHIFT) +
                       mat->m[3][0]) / PRES;
      
    result[index].y = (mat->m[0][1] * (points[index].x >> PSHIFT) +
                       mat->m[1][1] * (points[index].y >> PSHIFT) +
                       mat->m[2][1] * (points[index].z >> PSHIFT) +
                       mat->m[3][1]) / PRES;

    result[index].z = (mat->m[0][2] * (points[index].x >> PSHIFT) +
                       mat->m[1][2] * (points[index].y >> PSHIFT) +
                       mat->m[2][2] * (points[index].z >> PSHIFT) +
                       mat->m[3][2]) / PRES;
  }
}

void applyProjection(Vector2i *proj, const Vector3i *points, const unsigned int numPoints) {
  unsigned int index;

  for (index = 0; index < numPoints; index++) {
    int_t div = FOV + points[index].z;

    if (div != 0) {
      proj[index].x = (FOV * points[index].x) / (FOV + points[index].z) + HALFW;
      proj[index].y = -(FOV * points[index].y) / (FOV + points[index].z) + HALFH;
    }
    else {
      proj[index].x = (FOV * points[index].x) / PRES + HALFW;
      proj[index].y = -(FOV * points[index].y) / PRES + HALFH;
    }
  }
}

