#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
  int i;
  double mul = 256.0;

  if (argc > 1)
  {
    i = atoi(argv[1]);
    mul = (double) i;
  }

  printf("#include \"imath.h\"\n\n");
  printf("const fixed_t sintab_def[MAX_ANGLE + 1] = {\n  ");
  for (i = 0; i < 256; i++) {
    double val = mul * sin(M_PI * i / 128.0);
    if ((i % 8) == 0) printf("\n  ");
    printf("  %3d, ", (int) floor(val));
  }
  printf("\n};");
  printf("\n");

  printf("const fixed_t costab_def[MAX_ANGLE + 1] = {\n  ");
  for (i = 0; i < 256; i++) {
    double val = mul * cos(M_PI * i / 128.0);
    if ((i % 8) == 0) printf("\n  ");
    printf("  %3d, ", (int) floor(val));
  }
  printf("\n};");
  printf("\n");


  return 0;
}

