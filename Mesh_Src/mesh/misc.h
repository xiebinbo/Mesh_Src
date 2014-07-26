//
//    File: misc.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _MISC_H
#define _MISC_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <wctype.h>

//using namespace std;
#define  M_PI  3.14159
#define MAXFLOAT  999999.99999
#define TRUE 1
#define FALSE 0

#define TWOPI (2*M_PI)
#define PI M_PI
#define PI_2 (M_PI/2.0)
#define PI_4 (M_PI/4.0)

#define MIN(TYPE, A, B) ((TYPE) A < (TYPE) B ? A : B)
#define MAX(TYPE, A, B) ((TYPE) A > (TYPE) B ? A : B)

#define GRAD2RAD(G) (G/180.0*PI)
#define RAD2GRAD(R) (R/PI*180.0)

#define POSITIVE(A) (A > 0 ? A : 0)

#define FILE_ERROR(F, T)  (fprintf(stderr, "\n\nFile descriptor %d: %s\n\n", fileno(F), T))

#define SKIP_COMMENTS(F) \
{ \
  int c; \
\
  while ((c=fgetc(F)) == '#') \
  { \
    do \
    { \
      c=fgetc(F); \
    } \
    while (c != EOF && c != '\n'); \
  } \
\
  ungetc(c, F); \
}

#endif
