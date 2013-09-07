//
//    File: vertex.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "vertex.h"

Vertex::Vertex()
{ 
  data.set_zero(); 
  normal=NULL; 
  nr_edg=nr_tri=0; 
}

Vertex::Vertex(const Vertex &v)
{ 
  data=v.data; 
  normal=NULL; 
  nr_edg=nr_tri=0; 
}

Vertex::Vertex(float x, float y, float z)
{ 
  data.v[0]=x; 
  data.v[1]=y; 
  data.v[2]=z; 
  normal=NULL; 
  nr_edg=nr_tri=0; 
}

Vertex::~Vertex()
{
  delete normal;
}

const float* Vertex::float_data(void) const
{
  return data.v;
}

const MathVector* Vertex::math_data(void) const
{
  return &data;
}

float Vertex::x(void) const
{
  return data.v[0];
}

float Vertex::y(void) const
{
  return data.v[1];
}

float Vertex::z(void) const
{
  return data.v[2];
}

const float* Vertex::float_normal(void) const
{
  return normal != NULL ? normal->v : NULL;
}

void Vertex::add_normal(const MathVector *norm)
{
  if (normal == NULL)
    normal = new MathVector();

  normal->add(norm);
  normal->normalize();
}

void Vertex::rotate(const Matrix3<float> *r)
{
  float tmp[3];

  r->mul(data.v, tmp);

  data.v[0]=tmp[0];
  data.v[1]=tmp[1];
  data.v[2]=tmp[2];
}

int Vertex::equal(const Vertex *v) const
{
  return x() == v->x() && y() == v->y() && z() == v->z();
}

void Vertex::set(const Vertex *v)
{
  data.set(&v->data);
}

void Vertex::set(const MathVector *v)
{
  data.set(v);
}

void Vertex::move(float x, float y, float z)
{
  data.add(x, y, z);
}

void Vertex::move(const MathVector *v)
{
  data.add(v);
}

void Vertex::scale(float x, float y, float z)
{
  data.v[0]*=x;
  data.v[1]*=y;
  data.v[2]*=z;
}

void Vertex::scale(float scale)
{
  data.scale(1.0/scale);
}

float Vertex::length(void) const
{
  return data.length2();
}

void Vertex::addTriangle(Triangle* triangle)
{
  triangles.push_back(triangle);
  nr_tri++;
}

void Vertex::deleteTriangle(Triangle* triangle)
{
  triangles.remove(triangle);
  nr_tri--;
}

void Vertex::addEdge(Edge* edge)
{
  edges.push_back(edge);
  nr_edg++;
}

void Vertex::deleteEdge(Edge* edge)
{
  edges.remove(edge);
  nr_edg--;
}

list<Triangle*>* Vertex::get_triangles(void)
{
  return &triangles;
}

list<Edge*>* Vertex::get_edges(void)
{
  return &edges;
}

float Vertex::distance(const Vertex *v1, const Vertex *v2)
{
  MathVector d;
  MathVector::sub(&v1->data, &v2->data, &d);
  return d.length();
}

int Vertex::operator<(const Vertex &v) const
{
  for (int i=0; i < 3; i++)
    if (data.v[i] != v.data.v[i])
      return data.v[i] < v.data.v[i];

  return 0;
}
