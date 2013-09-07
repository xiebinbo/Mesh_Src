//
//    File: mesh.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _MESH_H
#define _MESH_H
#include <windows.h>

#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <stdio.h>

#include "shape.h"
#include "triangle.h"
#include "edge.h"
#include "vertex.h"

using namespace std;

typedef enum { LIST_MESH, P_MESH, GTS_MESH, VRML1_MESH, VRML2_MESH, PLY_MESH,
	       FEATURES_MESH, GEOMVIEW_MESH, UNKNOWN_MESH } Mesh_Type;

static int  strcasecmp(char* a, char* b)	//此函数只在Linux中提供，相当于windows平台的 stricmp
{
	return strcmp(a,b);
}

class Mesh
{
public:
  Mesh();
  virtual ~Mesh();
  
  void clear(void);

  void set_mesh(Mesh *mesh);
  void add_triangle(Triangle *t);
  void add_edge(Edge *e);
  void add_vertex(Vertex *v);
  void add_mesh(Mesh *mesh);
  void add_triangles(list<Triangle*> *new_triangles,
		     const char *texture_name = "", int clear_map = TRUE);
  void add_edges(list<Edge*> *new_edges, int clear_map = TRUE);
  void add_vertices(list<Vertex*> *new_vertices);

  void remove(Vertex *v);
  void remove(Edge *e);
  void remove(Triangle *t);

  Vertex* get_vertex(unsigned int name) const;
  Edge* get_edge(unsigned int name) const;
  Triangle* get_triangle(unsigned int name) const;

  list<Vertex*>* get_vertices(void) const;
  list<Edge*>* get_edges(void) const;
  list<Edge*>* get_edges(list<Edge*> *es) const;
  list<Triangle*>* get_triangles(void) const;
  Shape* get_shape(unsigned int no) const;

  static int type(FILE *f);

  virtual int read(FILE *f, int (*update_progress)(int pos),
		   void (*set_total)(int size));
  virtual void write(FILE *f, const char *comment = "");

  void write_points(FILE *f) const;
  void write_gts_points(FILE *f) const;

  void create_edges(void);

  int number_of_vertices(void) const;
  int number_of_edges(void) const;
  int number_of_shapes(void) const;
  int number_of_triangles(void) const;
  float average_triangle_size(void) const;
  float get_max_vertex_length(void) const;
  MathVector get_centroid(void) const;

  void move(const MathVector *v);
  void move_to_centre(void);
  void scale(float scale);
  void scale_into_normal_sphere(void);
  void calc_original_coordinates(const Vertex *v, Vertex *org) const;

  void negate_surface_normals(void);
  void remove_double_points(void);

  Vertex* find_closed_point(const Vertex *v) const;

  void clear_selection(void);
  Vertex* select_vertex(unsigned int name);
  Edge* select_edge(unsigned int name);
  Triangle* select_triangle(unsigned int name);

  list<Vertex*> selected_vertices;
  list<Edge*> selected_edges;
  list<Triangle*> selected_triangles;

protected:
  // helper function for createEdges
  Edge* get_edge(map< pair<Vertex*,Vertex*>, Edge* > *edge_map, 
		 Vertex *v1, Vertex *v2);

  // data
  vector<Shape*> *shapes;
  list<Triangle*> *triangles;
  list<Edge*> *edges;
  list<Vertex*> *vertices;
  int ver_nr, tri_nr, edge_nr;

  // a map to avoid adding vertices more than once
  // in add_triangles() & add_edges()
  map<Vertex*, Vertex*> vertex_map;

  MathVector model_centroid;
  float model_scale;
};

#endif
