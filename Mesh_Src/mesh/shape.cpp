//
//    File: shape.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include <set>
#include <algorithm>

#include "shape.h"

Shape::Shape(list<Triangle*> *tri, list<Vertex*> *ver, const char *text_name)
{
  triangles = tri;   edges = NULL;   vertices = ver;

  texture_id=-1;
  set_texture_name(text_name);
}

Shape::Shape(list<Triangle*> *tri, const char *text_name)
{
  triangles = tri;   edges = NULL;   vertices = new list<Vertex*>;

  texture_id=-1;
  set_texture_name(text_name);

  //create the vertex list from the triangle list
  set<Vertex*> vertex_set;
  list<Triangle*>::iterator it;
  int i;

  for (it=triangles->begin(); it != triangles->end(); it++)
    for (i = 0; i < 3; i++)
      // avoid double vertices
      if (vertex_set.find( (*it)->vertices[i] ) == vertex_set.end())
	{
	  vertices->push_back((*it)->vertices[i]);
	  vertex_set.insert((*it)->vertices[i]);
	}
}

Shape::Shape(list<Edge*> *edg, list<Vertex*> *ver)
{
  triangles = NULL;   edges = edg;   vertices = ver;

  texture_id=-1;
  set_texture_name(NULL);
}

Shape::Shape(list<Vertex*> *ver)
{
  triangles = NULL;   edges = NULL;   vertices = ver;

  texture_id=-1;
  set_texture_name(NULL);
}

Shape::~Shape()
{
  delete vertices;
  delete edges;
  delete triangles;
  delete texture_name;
}

int Shape::number_of_vertices(void) const
{
  return vertices != NULL ? vertices->size() : 0;
}

int Shape::number_of_edges(void) const
{
  return edges != NULL ? edges->size() : 0;
}

int Shape::number_of_triangles(void) const
{
  return triangles != NULL ? triangles->size() : 0;
}

int Shape::contains(list<Vertex*> *ver) const
{
  list<Vertex*>::iterator iv;

  for (iv=ver->begin(); iv != ver->end(); iv++)
    if ( find( vertices->begin(), vertices->end(), *iv ) == vertices->end() )
      return FALSE;

  return TRUE;
}

void Shape::set_texture_name(const char *text_name)
{
  if (text_name == NULL || strlen(text_name) == 0)
    texture_name = NULL;
  else
    {
      texture_name = new char[strlen(text_name)+1];
      strcpy(texture_name, text_name);
    }
}

void Shape::remove_vertex(Vertex *ver)
{
  if (vertices != NULL)
    vertices->remove(ver);
}

void Shape::change_vertex(const Vertex *old_ver, Vertex *new_ver)
{
  if (vertices != NULL)
    {
      list<Vertex*>::iterator iv;

      iv = find( vertices->begin(), vertices->end(), old_ver );
      if (iv != vertices->end())
	*iv = new_ver;
    }
}
