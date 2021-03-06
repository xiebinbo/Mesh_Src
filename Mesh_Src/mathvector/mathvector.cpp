//
//    File: mathvector.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "mathvector.h"

#define TINY 1.0e-12

MathVector angle_ref(1.0, 0.0, 0.0);

void MathVector::display(void) const
{
  printf("%f %f %f\n", v[0], v[1], v[2]);
}

void MathVector::set_angles(float alpha, float beta)
{
  Matrix3<float> R;

  R.set_rotation_from_angles(beta, 0.0, alpha);
  R.mul(angle_ref.v, v);
}

void MathVector::get_angles(float *alpha, float *beta) const
{
  MathVector a, b;
  float y;
 
  // get the vector and normalize it
  b=*this;
  b.normalize();
 
  // calc y for 1st rotation
  y = sqrt(1.0 - b.v[0]*b.v[0]);
 
  // vector for 1st rotation
  a.set(b.v[0], y, 0.0);

  // 1st rotation
  *alpha= angle(&a, &angle_ref);

  // 2st rotation
  a.v[0] = b.v[0] = TINY;
  a.normalize(); b.normalize();
  *beta= v[2] > 0.0 ? angle(&a, &b) : -angle(&a, &b);
}

double MathVector::length() const
{
  return sqrt( length2() );
}

double MathVector::length2() const
{
  return (double) v[0]*v[0] + (double) v[1]*v[1] + (double) v[2]*v[2];
}

void MathVector::set_zero(void)
{
  v[0] = v[1] = v[2] = 0.0;
}

void MathVector::set(float x, float y, float z)
{
  v[0]=x; v[1]=y; v[2]=z;
}

void MathVector::set(const MathVector *a)
{
  set(a->v[0], a->v[1], a->v[2]);
}

void MathVector::add(float x, float y, float z)
{
  v[0]+=x;
  v[1]+=y;
  v[2]+=z;
}

void MathVector::add(const MathVector *a)
{
  add(a->v[0], a->v[1], a->v[2]);
}

void MathVector::sub(const MathVector *a)
{
  add(-a->v[0], -a->v[1], -a->v[2]);
}

void MathVector::normalize()
{
  scale(1.0/length());
}

void MathVector::negation(void)
{
  scale(-1.0);
}

void MathVector::square(void)
{
  v[0]*=v[0];
  v[1]*=v[1];
  v[2]*=v[2];
}

void MathVector::mul(float m)
{
  scale(m);
}

void MathVector::dif(float m)
{
  scale(1.0/m);
}

void MathVector::scale(double scale)
{
  v[0]*=scale;
  v[1]*=scale;
  v[2]*=scale;
}

int MathVector::operator<(const MathVector &v2) const
{
  for (int i=0; i < 3; i++)
    if (v[i] != v2.v[i])
      return v[i] < v2.v[i];

  return 0;
}

double MathVector::length(const MathVector *v1, const MathVector *v2)
{
  MathVector c;
  
  sub(v1, v2, &c);

  return c.length();
}

double MathVector::length2(const MathVector *v1, const MathVector *v2)
{
	MathVector c;

	sub(v1, v2, &c);

	return c.length2();
}

void MathVector::add(const MathVector *v1, const MathVector *v2, 
		     MathVector *result)
{
  result->v[0] = v1->v[0] + v2->v[0];
  result->v[1] = v1->v[1] + v2->v[1];
  result->v[2] = v1->v[2] + v2->v[2];
}

void MathVector::sub(const MathVector *v1, const MathVector *v2, 
		     MathVector *result)
{
  result->v[0] = v1->v[0] - v2->v[0];
  result->v[1] = v1->v[1] - v2->v[1];
  result->v[2] = v1->v[2] - v2->v[2];
}

void MathVector::copy(const MathVector *from, MathVector *to)
{
  to->v[0] = from->v[0];
  to->v[1] = from->v[1];
  to->v[2] = from->v[2];
}

double MathVector::dot_product(const MathVector *v1, const MathVector *v2)
{
  return (double) v1->v[0]*v2->v[0] +
         (double) v1->v[1]*v2->v[1] +
         (double) v1->v[2]*v2->v[2]; 
}

void MathVector::cross_product(const MathVector *v1, const MathVector *v2,
			       MathVector *result)
{
  result->v[0] = v1->v[1]*v2->v[2] - v1->v[2]*v2->v[1];
  result->v[1] = v1->v[2]*v2->v[0] - v1->v[0]*v2->v[2];
  result->v[2] = v1->v[0]*v2->v[1] - v1->v[1]*v2->v[0];
}

int MathVector::bigger_90_degrees(const MathVector *v1, const MathVector *v2)
{
  return dot_product(v1, v2) < 0.0;
}

float MathVector::angle(const MathVector *v1, const MathVector *v2)
{
  return acos( dot_product(v1, v2) / (v1->length()*v2->length()) );
}

float MathVector::angle_90(const MathVector *v1, const MathVector *v2)
{
  return acos( fabs(dot_product(v1, v2)) / (v1->length()*v2->length()) );
}

Quaternion::Quaternion()
{
  v.set_zero(); s = 0.0;

  R.set_identity();
}

Quaternion::Quaternion(const MathVector *vv, float ss)
{
  v = *vv;
  s = ss;

  R.set_rotation_from_quaternion(-v.v[0], -v.v[1], -v.v[2], s);
}

Quaternion::Quaternion(const MathVector *from, const MathVector *to)
{
  MathVector f = *from;
  MathVector t = *to;

  f.normalize();
  t.normalize();

  double rotation_angle = MathVector::angle_90(&f, &t);
  
  // vectors are parallel ?
  if (rotation_angle > 0.0)
    {
      MathVector::cross_product(&f, &t, &v);
      v.normalize();
      v.mul(sin(rotation_angle/2.0)); 
      s = cos(rotation_angle/2.0);
   }
  else
    {
      v.set_zero();
      s = 0;
    }

  R.set_rotation_from_quaternion(-v.v[0], -v.v[1], -v.v[2], s);

  /*
  printf("XXX\n");

  MathVector tmp;

  f.display();
  t.display();

  R.mul(f->v, tmp.v);
  tmp.display();

  printf("XXX\n");
  */
}

MathVector Quaternion::rotate(const MathVector *from) const
{
  MathVector to;

  R.mul(from->v, to.v);

  return to;
}
