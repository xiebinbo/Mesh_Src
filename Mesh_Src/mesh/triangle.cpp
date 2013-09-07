//
//    File: triangle.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "triangle.h"

Triangle::Triangle(Vertex *v1, Vertex *v2, Vertex *v3)
{
  int v;

  plane=0;

  vertices[0]=v1;
  vertices[1]=v2;
  vertices[2]=v3;

  edges[0]=edges[1]=edges[2]=NULL;

  calc_properties();

  for (v=0; v < 3; v++)
    {
      vertices[v]->addTriangle(this);
      vertices[v]->add_normal(&surface_normal);
    }
}

Triangle::Triangle(Edge *e1, Edge *e2, Edge *e3)
{
  int e, v, j, nr_ver, found;

  plane=0;

  edges[0]=e1;
  edges[1]=e2;
  edges[2]=e3;

  for (e=0; e < 3; e++)
    edges[e]->addTriangle(this);

  if ( (edges[0]->vertices[0] != edges[1]->vertices[0]) &&
       (edges[0]->vertices[0] != edges[1]->vertices[1]) )
  {
      vertices[0]=edges[0]->vertices[0];
      vertices[1]=edges[0]->vertices[1];
  }
  else
  {
      vertices[0]=edges[0]->vertices[1];
      vertices[1]=edges[0]->vertices[0];
  }

  if ( (edges[1]->vertices[0] != edges[0]->vertices[0]) &&
       (edges[1]->vertices[0] != edges[0]->vertices[1]) )
  {
      vertices[2]=edges[1]->vertices[0];
  }
  else
  {
      vertices[2]=edges[1]->vertices[1];
  }

  calc_properties();

  for (v=0; v < 3; v++)
    {
      vertices[v]->addTriangle(this);
      vertices[v]->add_normal(&surface_normal);
    }
}

void Triangle::set_vertices(const MathVector *v1, const MathVector *v2,
			    const MathVector *v3)
{
  vertices[0]->set(v1);
  vertices[1]->set(v2);
  vertices[2]->set(v3);

  calc_properties();
}

void Triangle::calc_properties(void)
{
  // calculate surface centroid and register triangle at the vertices
  surface_centroid.set_zero();
  for (int v=0; v < 3; v++)
    surface_centroid.add(vertices[v]->math_data());
  surface_centroid.scale(1.0/3.0);

  // calculate the vectors between the vertices
  MathVector c[3];

  MathVector::sub(vertices[0]->math_data(), vertices[1]->math_data(), &c[0]);
  MathVector::sub(vertices[0]->math_data(), vertices[2]->math_data(), &c[1]);
  MathVector::sub(vertices[2]->math_data(), vertices[1]->math_data(), &c[2]);

  // calculate surface normal
  MathVector::cross_product(&c[0], &c[1], &surface_normal);
  surface_normal.normalize();

  // calculate distance to the origin from the centroid in direction
  // of the surface normal
  surface_distance_to_origin = MathVector::dot_product(&surface_centroid,
    						       &surface_normal);
  // calculate surface perimeter
  surface_perimeter = c[0].length() + c[1].length() + c[2].length();

  // calculate surface size
  float s=surface_perimeter/2.0;
  surface_size=sqrt(s * (s-c[0].length()) * (s-c[1].length()) * (s-c[2].length()));
}

Triangle::~Triangle()
{
  for (int i=0; i < 3; i++)
    {
      vertices[i]->deleteTriangle(this);
      if (edges[i] != NULL)
	edges[i]->deleteTriangle(this);
    }
}

void Triangle::moveCentroid(const MathVector *v)
{
  surface_centroid.add(v);
}

void Triangle::negate_normal(void)
{
  Vertex *dummy;

  dummy=vertices[1];
  vertices[1]=vertices[2];
  vertices[2]=dummy;

  surface_normal.negation();
  surface_distance_to_origin = -surface_distance_to_origin;
}

const float* Triangle::centroid(void) const
{
  return surface_centroid.v;
}

const MathVector* Triangle::math_centroid(void) const
{
  return &surface_centroid;
}

const float* Triangle::float_normal(void) const
{
  return surface_normal.v;
}

const MathVector* Triangle::math_normal(void) const
{
  return &surface_normal;
}

float Triangle::size(void) const
{
  return surface_size;
}

float Triangle::perimeter(void) const
{
  return surface_perimeter;
}

float Triangle::distance_to_origin(void) const
{
  return surface_distance_to_origin;
}

float Triangle::angle(const Vertex *v) const
{
  MathVector c[3];
  int i, k;

  // find the two vectors between the vertices
  for (i=k=0; i < 3; i++)
    if (v != vertices[i])
      MathVector::sub(v->math_data(), vertices[i]->math_data(), &c[k++]);

  // calculate the angle between the edges
  return k == 2 ? MathVector::angle_90(&c[0], &c[1]) : 0;
}

vector<Triangle*> Triangle::neighbors(void) const
{
  list<Triangle*> *edge_triangles;
  list<Triangle*>::iterator it;
  vector<Triangle*> nei;

  for (int i=0; i < 3; i++)
    {
      edge_triangles=&edges[i]->triangles;
      for (it=edge_triangles->begin(); it != edge_triangles->end(); it++)
	if (!equal(*it))
	  nei.push_back(*it);
    }

  return nei;
}

vector<Triangle*> Triangle::neighbors_on_plane(void) const
{
  vector<Triangle*> nei = neighbors();
  vector<Triangle*>::iterator it;

  for (it=nei.begin(); it != nei.end(); )
    if ((*it)->plane == plane)
      it++;
    else
      it=nei.erase(it);
  
  return nei;
}

int Triangle::get_surrounding_plane(void) const
{
  vector<Triangle*> nei = neighbors();
  int i, k, size = nei.size();

  if (size == 1)
    return nei[0]->plane;

  // look for two which are the same
  for (i=0; i < size-1; i++)
    for (k=i+1; k < size; k++)
      if (nei[i]->plane == nei[k]->plane)
	return nei[i]->plane;

  return 0;
}

int Triangle::change_vertex(const Vertex *old_v, Vertex *new_v)
{
  int i=0;

  while (i < 3 && vertices[i] != old_v)
    i++;

  if (i != 3)
    {
      vertices[i]=new_v;
      return 1;
    }

  return 0;
}

void Triangle::set_text_coordinates(int i, float s, float t)
{
  text_s[i] = s;
  text_t[i] = t;
}

void Triangle::set_text_coordinates(int i, const pair<float, float>* texCoord)
{
  text_s[i] = texCoord->first;
  text_t[i] = texCoord->second;
}

float Triangle::get_text_t(int i) const
{
  return text_t[i];
}

float Triangle::get_text_s(int i) const
{
  return text_s[i];
}

int Triangle::equal(const Triangle *tri) const
{
  return vertices[0] == tri->vertices[0] &&
    vertices[1] == tri->vertices[1] && vertices[2] == tri->vertices[2];
}

int Triangle::on_same_plane(const Triangle *tri, float distance_tolerance,
			    float orientation_tolerance) const
{
  MathVector normal2 = tri->surface_normal;
  float distance2 = MathVector::dot_product(&tri->surface_centroid,
					    &surface_normal);

  // triangle facing in different directions?
  if (MathVector::angle_90(&surface_normal, &normal2) > 
      GRAD2RAD(orientation_tolerance))
    return 0;

  // optimisation ?!?!
  //
  //  if (MathVector::angle_90(&surface_normal, &normal2) > GRAD2RAD(20.0))
  //
  // because the normals are already normalised
  //  if (acos(MathVector::dot_product(&surface_normal, &normal2)) > GRAD2RAD(20.0))
  //
  // one function call less
  //  if (acos(MathVector::dot_product(&surface_normal, &normal2)) > GRAD2RAD(20.0))
  //
  // move cos on other side
  //  if (MathVector::dot_product(&surface_normal, &normal2) < cos(GRAD2RAD(20.0)))

  float d = surface_distance_to_origin - distance2;

  // the centroids are on the same plane?
  return fabs(d)*100 < distance_tolerance;
}
