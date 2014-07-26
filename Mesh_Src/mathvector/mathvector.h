//
//    File: mathvector.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _MATHVECTOR_H
#define _MATHVECTOR_H

#include <stdio.h>
#include <math.h>
#include <misc.h>

#include "matrix3.h"

// Vector
class MathVector
{
public:
  MathVector()
    { set_zero(); }
  MathVector(float x, float y, float z)
    { set(x, y, z); }

  void display(void) const;
  void set(float x, float y, float z);
  void set(const MathVector *a);
  void set_angles(float alpha, float beta);
  void get_angles(float *alpha, float *beta) const;
  double length(void) const;
  double length2(void) const;
  void set_zero(void);
  void add(float x, float y, float z);
  void add(const MathVector *a);
  void sub(const MathVector *a);
  void mul(float m);
  void dif(float m);
  void square(void);
  void scale(double scale);
  void negation(void);
  void normalize(void);

  int operator<(const MathVector &v2) const;

  static double length(const MathVector *v1, const MathVector *v2);
  static double length2(const MathVector *v1, const MathVector *v2);
  static void add(const MathVector *v1, const MathVector *v2, 
		  MathVector *result);
  static void sub(const MathVector *v1, const MathVector *v2, 
		  MathVector *result);
  static void copy(const MathVector *from, MathVector *to);

  static double dot_product(const MathVector *v1, const MathVector *v2);
  static void cross_product(const MathVector *v1, const MathVector *v2,
			    MathVector *result);

  static float angle(const MathVector *v1, const MathVector *v2);
  static float angle_90(const MathVector *v1, const MathVector *v2);
  static int bigger_90_degrees(const MathVector *v1, const MathVector *v2);

  float v[3];
};

// Quaternion
class Quaternion
{
 public:
  Quaternion();
  Quaternion(const MathVector *vv, float ss);
  Quaternion(const MathVector *from, const MathVector *to);

  MathVector rotate(const MathVector *v) const;
  void display(void) const { printf("%f ", s); v.display(); }

 private:
  Matrix3<float> R;
  MathVector v;
  float s;
};

#endif
