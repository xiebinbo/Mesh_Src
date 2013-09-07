//
//    File: triangle.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <vector>
#include "edge.h"
#include "vertex.h"
#include "mathvector.h"
#include "misc.h"

using namespace std;

class Triangle
{
public:
  Triangle(Vertex *v1, Vertex *v2, Vertex *v3);
  Triangle(Edge *e1, Edge *e2, Edge *e3);
  ~Triangle();

  //  void set(MathVector *v1, MathVector *v2, MathVector *v3);
  void calc_properties(void);
  void set_vertices(const MathVector *v1, const MathVector *v2, 
		    const MathVector *v3);
  int change_vertex(const Vertex *old_v, Vertex *new_v);
  void moveCentroid(const MathVector *v);
  void negate_normal(void);

  void set_text_coordinates(int i, float s, float t);
  void set_text_coordinates(int i, const pair<float, float> *texCoord);
  float get_text_t(int i) const;
  float get_text_s(int i) const;

  const float* centroid(void) const;
  const MathVector* math_centroid(void) const;
  const float* float_normal(void) const;
  const MathVector* math_normal(void) const;

  float size(void) const;
  float perimeter(void) const;
  float distance_to_origin(void) const;
  float angle(const Vertex *v) const;

  int equal(const Triangle *tri) const;
  int on_same_plane(const Triangle *tri, float distance_tolerance,
		    float orientation_tolerance) const;

  vector<Triangle*> neighbors(void) const;
  vector<Triangle*> neighbors_on_plane(void) const;
  int get_surrounding_plane(void) const;

  Edge *edges[3];
  Vertex *vertices[3];
  unsigned int plane, name;

private:
  MathVector surface_normal, surface_centroid;
  float surface_size, surface_perimeter, surface_distance_to_origin;
  float text_s[3], text_t[3];
};

#endif
