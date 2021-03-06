//
//    File: vertex.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _VERTEX_H
#define _VERTEX_H

#include <list>
#include <utility>

#include "triangle.h"
#include "edge.h"
#include "matrix3.h"
#include "mathvector.h"

using namespace std;

class Triangle;
class Edge;

class Vertex
{
public:
  Vertex();
  Vertex(const Vertex &v);
  Vertex(float x, float y, float z);
  ~Vertex();

  void addTriangle(Triangle* triangle);
  void deleteTriangle(Triangle* triangle);
  void addEdge(Edge* edge);
  void deleteEdge(Edge* edge);

  float x(void) const;
  float y(void) const;
  float z(void) const;
  const float* float_data(void) const;
  const MathVector* math_data(void) const;
  const MathVector* math_newdata(void) const;
  const MathVector* math_lapcoor(void) const;
  MathVector* math_quantized_lapcoor(void);

  const float* float_normal(void) const;
  void add_normal(const MathVector *norm);

  float length(void) const;
  void set(const Vertex *v);
  void set(const MathVector *v);
  void move(const MathVector *v);
  void move(float x, float y, float z);
  void scale(float x, float y, float z);
  void scale(float scale);
  void rotate(const Matrix3<float> *r);
  int equal(const Vertex *v) const;

  list<Triangle*>* get_triangles(void);
  list<Edge*>* get_edges(void);

  static float distance(const Vertex *v1, const Vertex *v2);

  int operator<(const Vertex &v) const;
	void get_neighborvertexs(void);
	MathVector* ucmath_data(void);
	void computer_GaussCurvature(void);
	void compute_square_devaition_angle(void);
	double geometric_laplacian(void);
  unsigned int name, number,blockid;
  bool is_segment_traversed;
  bool is_laplacian_lost;
  bool is_visited;
  int to_anthor_length;
  int color[3];
  int dye;//dye==-1表示未着色
  float curvature;
  float square_devaition_angle;
  double Sq;
  double Mq;
  //xbb
  list<Vertex*> neighborvertex;//存放邻接点
  list<Vertex*> segmented_neighborvertex;//分割之后顶点的邻接点
  list<Vertex*> segmented_neighborvertex2;
  MathVector quantized_lapcoor;
  MathVector newdata;//存储新的坐标
  MathVector lapcoor;//laplacian 差值

private:
  MathVector *normal, data;
  int nr_tri, nr_edg;
  list<Edge*> edges;
  list<Triangle*> triangles;
};

#endif
