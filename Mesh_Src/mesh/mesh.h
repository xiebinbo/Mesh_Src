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
#include <set>
#include <queue>
#include <algorithm>
#include <stdio.h>
#include <cmath>
#include "shape.h"
#include "triangle.h"
#include "edge.h"
#include "vertex.h"
#include <time.h>
#include <iostream>  
#include <Eigen/Dense> 
#include <Eigen/LU>
#include <Eigen/Sparse>
  
using namespace Eigen;   
using namespace std; 

typedef Eigen::SparseMatrix<float> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<float> T;

typedef enum { LIST_MESH, P_MESH, GTS_MESH, VRML1_MESH, VRML2_MESH, PLY_MESH,
	       FEATURES_MESH, GEOMVIEW_MESH, UNKNOWN_MESH } Mesh_Type;

static int  strcasecmp(char* a, char* b)	//此函数只在Linux中提供，相当于windows平台的 stricmp
{
	return strcmp(a,b);
}

string convertToString(int x);

bool Sqlarge(const Vertex* v1, const Vertex* v2);
typedef Eigen::Triplet<float> T;

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
  void mymove_to_centre(void);
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
  //xbb
	void initialize_delta(void);
	void ini_ner_ver(void);
	void solve_invertible_linear_equations(void);
	void solve_sparse_linear_equations(void);
	void lookfor_seeds(int num);
	void segment_graph(void);
	void ini_segmented_ner_ver(void);//分块后修正每个顶点的邻接点
	void ini_segmented_nei2_ver(void);//分块后topo-2
	void block_least_square(int bit,int anchor);
	void solving_least_squares(int anchor);
	void greedy_packet(void);
	void lost_laplacian_coordinates(void);
	void compute_vertex_curvature(void);
	void compute_vertex_square_devaition_angle(void);
	void quantizing_laplacian_coordinates(int bit);
	double visual_quality_measure(float a);
	void visual_quality_measure_for_greedy(void);
	void Hausdorff_distance(void);
	void hausdorff_distance_1(void);
	void initialize_tripletList_and_deltamesh(void);
	void iterator_soving_least_square(int anchor);
	void iterator_compute_number(int num);
	void replace_better_anchor(FILE *f,int num);
	void statistics_data(FILE *f,int bit);
	void ccccc(void);
	void test_anchor(int bit, fstream &fs, string &ss);
	void test_anchor1(void);
	void test_anchor2(void);
	void test_anchor3(void);
	void test_anchor4(void);
	void BFS_Graph(Vertex *anchor, int i);
  list<Vertex*> selected_vertices;
  list<Edge*> selected_edges;
  list<Triangle*> selected_triangles;
	MatrixXf deltamesh;
	list<Vertex*> seeds;
	vector<list<Vertex*>> superlist;
	list<Vertex*> *li;
	vector<list<Vertex*>*> supervector;
	vector<Vertex*> vesq;
	vector<T> tripletList;
	double the_min;
	Vertex* first_anchor;
	vector<double> cevis;
	vector<Vertex*> ver;
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
