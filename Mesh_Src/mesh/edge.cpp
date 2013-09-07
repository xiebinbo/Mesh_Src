//
//    File: edge.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "triangle.h"
#include "edge.h"

Edge::Edge(Vertex *v1, Vertex *v2)
{
  vertices[0]=v1;   v1->addEdge(this);
  vertices[1]=v2;   v2->addEdge(this);


  nr_tri=0;

  calc_properties();
}

Edge::~Edge()
{
  for (int i=0; i < 2; i++)
    vertices[i]->deleteEdge(this);
}

void Edge::set(const MathVector *v1, const MathVector *v2)
{
  vertices[0]->set(v1);
  vertices[1]->set(v2);

  calc_properties();
}

void Edge::calc_properties(void)
{
  edge_centroid.set_zero();
  for (int v=0; v < 2; v++)
    edge_centroid.add(vertices[v]->math_data());
  edge_centroid.scale(1.0/2.0);

  // connection between the vertices
  MathVector::sub(vertices[0]->math_data(), vertices[1]->math_data(), 
		  &edge_orientation);
  // length of the connection
  edge_length = edge_orientation.length();
  // connection => orientation
  edge_orientation.normalize();
}

int Edge::equal(const Vertex *v1, const Vertex *v2) const
{
  if ((vertices[0] == v1 && vertices[1] == v2) ||
      (vertices[0] == v2 && vertices[1] == v1))
    return TRUE;
  else
    return FALSE;
}

int Edge::equal(const Edge *e) const
{
  if (vertices[0]->equal(e->vertices[0]) && vertices[1]->equal(e->vertices[1]))
    return TRUE;
  if (vertices[0]->equal(e->vertices[1]) && vertices[1]->equal(e->vertices[0]))
    return TRUE;

  return FALSE;
}

int Edge::change_vertex(const Vertex *old_v, Vertex *new_v)
{
  int i=0;

  while (i < 2 && vertices[i] != old_v)
    i++;

  if (i != 2)
    {
      vertices[i]=new_v;
      return 1;
    }

  return 0;
}

int Edge::on_same_edge(const Edge *e, float tolerance) const
{
  // Check if both GPOINTS from e are on the edge

  MathVector point, ori;
  float distance[2];

  //  if (MathVector::angle2(&edge_orientation, e->math_orientation())
  //      >= GRAD2RAD(70.0))
  //    return 0;

  for (int i=0; i < 2; i++)
    {
      ori=edge_orientation;
      point.set(e->vertices[i]->math_data());
      point.sub(vertices[0]->math_data());
      ori.scale(MathVector::dot_product(&point, &ori));
      point.sub(&ori);
      distance[i]=point.length();

    }  

  return distance[0] <= tolerance && distance[1] <= tolerance;
}

void Edge::addTriangle(Triangle* triangle)
{
  triangles.push_back(triangle);
  nr_tri++;
}

void Edge::deleteTriangle(Triangle* triangle)
{
  triangles.remove(triangle);
  nr_tri--;
}

const MathVector* Edge::math_orientation(void) const
{
  return &edge_orientation;
}

float Edge::length(void) const
{
  return edge_length;
}

const float* Edge::centroid(void) const
{
  return edge_centroid.v;
}

const MathVector* Edge::math_centroid(void) const
{
  return &edge_centroid;
}
