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
  dye = -1;
  to_anthor_length = 99999;
  is_segment_traversed=is_laplacian_lost=false;
  is_visited = false;
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

const MathVector* Vertex::math_newdata(void) const
{
	return &newdata;
}

const MathVector* Vertex::math_lapcoor(void) const
{
	return &lapcoor;
}

MathVector* Vertex::math_quantized_lapcoor(void)
{
	return &quantized_lapcoor;
}

MathVector* Vertex::ucmath_data()
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

void Vertex::get_neighborvertexs(void)
{
	list<Edge*> *neighborsedges;
	list<Edge*>::iterator it;

	list<Vertex*>::iterator vit;
	list<Vertex*>::iterator vit2;

	neighborsedges = &edges;

	for (it=neighborsedges->begin(); it != neighborsedges->end(); it++)
	{
		if ((*it)->vertices[0]->equal(this))
		{
			neighborvertex.push_back( (*it)->vertices[1]);
		}
		else
		{
			neighborvertex.push_back((*it)->vertices[0]);
		}  
	}
}

void Vertex::computer_GaussCurvature()
{
	list<Triangle*>* tri;
	tri = get_triangles();
	float area=0;
	float _angle=0;
	for(list<Triangle*>::iterator it = tri->begin();it != tri->end();it++)
	{
		area += (*it)->size();
		_angle += (*it)->angle(this);
	}
	//printf("%f %f",area, _angle);
	curvature = (TWOPI - _angle);
}

void Vertex::compute_square_devaition_angle()
{
	list<Edge*> *_edge;
	Triangle *tri1,*tri2;
	list<Triangle*>::iterator _ltri;
	_edge = get_edges();
	float _angle;
	double fabfcha=0;
	int count=0;
	for(list<Edge*>::iterator ie = _edge->begin();ie != _edge->end();ie++)
	{
		if((*ie)->triangles.size()==2)
		{
			
			_ltri = (*ie)->triangles.begin();
			tri1 = (*_ltri);
			_ltri++;
			tri2 = (*_ltri);
			(*ie)->_angle = acos(MathVector::dot_product(tri1->math_normal(),tri2->math_normal())/(tri1->math_normal()->length()*tri2->math_normal()->length()));
			_angle += (*ie)->_angle;
			count++;
		}
	}
	_angle /= count;
	for(list<Edge*>::iterator ie = _edge->begin();ie != _edge->end();ie++)
	{
		if((*ie)->triangles.size()==2)
		{
			fabfcha += pow((*ie)->_angle - _angle,2);
		}
	}
	this->square_devaition_angle = sqrt(fabfcha);
}

double Vertex::geometric_laplacian()
{
	list<Vertex*>::iterator it;
	float l;
	MathVector c, c1;
	MathVector sum, sumn;
	float sl = 0;
	float sln = 0;
	sum.set_zero();
	sumn.set_zero();
	for(it = neighborvertex.begin();it != neighborvertex.end(); it++)
	{
		l = 1.0/MathVector::length(this->math_data(),(*it)->math_data());
		sl += l;
		c.set((*it)->math_data());
		c.scale(l);
		sum.add(&c);

		l = 1.0/MathVector::length(this->math_newdata(),(*it)->math_newdata());
		sln += l;
		c.set((*it)->math_newdata());
		c.scale(l);
		sumn.add(&c);
	}
	sum.scale(1.0/sl);
	sum.sub(this->math_data());
	sum.negation();

	sumn.scale(1.0/sln);
	sumn.sub(this->math_newdata());
	sumn.negation();

	return MathVector::length2(&sum, &sumn);
}
