//
//    File: edge.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _EDGE_H
#define _EDGE_H
#include <windows.h>
#include <list>

#include "triangle.h"
#include "vertex.h"
#include "mathvector.h"

using namespace std;

class Vertex;
class Triangle;

class Edge
{
 public:
  Edge(Vertex *v1, Vertex *v2);
  ~Edge();

  void set(const MathVector *v1, const MathVector *v2);
  void calc_properties(void);
  void addTriangle(Triangle* triangle);
  void deleteTriangle(Triangle* triangle);
  int change_vertex(const Vertex *old_v, Vertex *new_v);

  int on_same_edge(const Edge *e, float tolerance) const;
  int equal(const Edge *e) const;
  int equal(const Vertex *v1, const Vertex *v2) const;

  float length(void) const;
  const float* centroid(void) const;
  const MathVector* math_centroid(void) const;
  const MathVector* math_orientation(void) const;
  void compute_angle(void);

  Vertex *vertices[2];
  list<Triangle*> triangles;

  unsigned int nr_tri, number, name;
  float _angle;//边所在的两三角形的夹角

 private:
  float edge_length;
  MathVector edge_centroid, edge_orientation;
};

#endif

