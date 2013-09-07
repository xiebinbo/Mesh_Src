//
//    File: shape.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _SHAPE_H
#define _SHAPE_H

#include <list>

#include "triangle.h"
#include "edge.h"
#include "vertex.h"

using namespace std;

class Shape
{
 public:
  Shape(list<Triangle*> *tri, list<Vertex*> *ver, const char *text_name = "");
  Shape(list<Triangle*> *tri, const char *text_name = "");
  Shape(list<Edge*> *edg, list<Vertex*> *ver);
  Shape(list<Vertex*> *ver);
  ~Shape();

  int get_texture_id() const { return texture_id; }
  void set_texture_id(int id) { texture_id = id; }
  const char* get_texture_name() const { return texture_name; }

  int number_of_vertices(void) const;
  int number_of_edges(void) const;
  int number_of_triangles(void) const;

  int contains(list<Vertex*> *vertices) const;
  void remove_vertex(Vertex *ver);
  void change_vertex(const Vertex *old_ver, Vertex *new_ver);
  void set_texture_name(const char *text_name);

  list<Triangle*> *triangles;
  list<Edge*> *edges;
  list<Vertex*> *vertices;

 private:
  int texture_id;
  char *texture_name;
};

#endif
