//
//    File: mesh.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

// for MAXFLOAT
//#include <values.h>

#include "mesh.h"

Mesh::Mesh()
{
	edge_nr = ver_nr = tri_nr = 0;

	model_centroid.set_zero();
	model_scale = 1.0;

	shapes = new vector<Shape*>;
	triangles = new list<Triangle*>;
	vertices = new list<Vertex*>;
	edges = new list<Edge*>;
}

Mesh::~Mesh()
{
	printf("destructor...............\n");
	clear();

	delete shapes;
	delete triangles;
	delete edges;
	delete vertices;
}

void Mesh::clear(void)
{
	vector<Shape*>::iterator is;
	list<Triangle*>::iterator it;
	list<Edge*>::iterator ie;
	list<Vertex*>::iterator iv;

	for (is=shapes->begin(); is != shapes->end(); is++)
		delete *is;
	for (it=triangles->begin(); it != triangles->end(); it++)
		delete *it;
	for (ie=edges->begin(); ie != edges->end(); ie++)
		delete *ie;
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		delete *iv;

	shapes->clear();
	triangles->clear();
	edges->clear();
	vertices->clear();

	edge_nr = ver_nr = tri_nr = 0;
}

int Mesh::number_of_vertices(void) const
{
	return ver_nr;
}

int Mesh::number_of_edges(void) const
{
	return edge_nr;
}

int Mesh::number_of_triangles(void) const
{
	return tri_nr;
}

void Mesh::add_triangle(Triangle *t)
{
	triangles->push_back(t);
	t->name = ++tri_nr;
}

void Mesh::add_edge(Edge *e)
{
	edges->push_back(e);
	e->name = ++edge_nr;
}

void Mesh::add_vertex(Vertex *v)
{
	vertices->push_back(v);
	v->name = ++ver_nr;
}

void Mesh::remove(Vertex *v)
{
	vector<Shape*>::iterator is;

	vertices->remove(v);
	for (is=shapes->begin(); is != shapes->end(); is++)
		(*is)->vertices->remove(v);

	delete v;
}

void Mesh::remove(Edge *e)
{
	vector<Shape*>::iterator is;

	edges->remove(e);
	for (is=shapes->begin(); is != shapes->end(); is++)
		if ((*is)->edges != NULL)
			(*is)->edges->remove(e);

	delete e;
}

void Mesh::remove(Triangle *t)
{
	vector<Shape*>::iterator is;

	triangles->remove(t);
	for (is=shapes->begin(); is != shapes->end(); is++)
		(*is)->triangles->remove(t);

	delete t;
}

void Mesh::add_vertices(list<Vertex*> *new_vertices)
{
	list<Vertex*>::iterator iv;
	list<Vertex*> *shape_vertices;
	Vertex *ver;

	shape_vertices = new list<Vertex*>;

	for (iv=new_vertices->begin(); iv != new_vertices->end(); iv++)
	{
		ver = new Vertex( (*iv)->x(), (*iv)->y(), (*iv)->z() );
		add_vertex(ver);
		shape_vertices->push_back(ver);
	}

	shapes->push_back(new Shape(shape_vertices));
}

void Mesh::add_edges(list<Edge*> *new_edges, int clear_map)
{
	// mapping from old to new vertices
	map<Vertex*, Vertex*>::iterator iv;
	list<Edge*>::iterator ie;
	list<Vertex*> *shape_vertices;
	list<Edge*> *shape_edges;
	Vertex *ver, *v[2];
	Edge *edg;
	int i;

	shape_vertices = new list<Vertex*>;
	shape_edges = new list<Edge*>;

	for (ie=new_edges->begin(); ie != new_edges->end(); ie++)
	{
		// Get or create the two new vertices
		for (i=0; i < 2; i++)
		{
			ver=(*ie)->vertices[i];
			// avoid using vertices more than once
			iv=vertex_map.find(ver);
			if (iv == vertex_map.end())
			{
				// Create one new vertex
				v[i] = new Vertex( ver->x(), ver->y(), ver->z() );
				add_vertex(v[i]);
				shape_vertices->push_back(v[i]);
				// old vertex => new vertex
				vertex_map[ver] = v[i];
			}
			else
				// We found the vertex in our list
				v[i]=(*iv).second;
		}

		// Create one new edge with the two vertices
		edg = new Edge(v[0], v[1]);
		add_edge(edg);
		shape_edges->push_back(edg);
	}

	shapes->push_back(new Shape(shape_edges, shape_vertices));

	if (clear_map)
		vertex_map.clear();
}

void Mesh::add_triangles(list<Triangle*> *new_triangles, 
						 const char *texture_name, int clear_map)
{
	// mapping from old to new vertices
	map<Vertex*, Vertex*>::iterator iv;
	list<Triangle*>::iterator it;
	list<Triangle*> *shape_triangles;
	list<Vertex*> *shape_vertices;
	Vertex *ver, *v[3];
	Triangle *tri;
	int i;

	shape_triangles = new list<Triangle*>;
	shape_vertices = new list<Vertex*>;

	for (it=new_triangles->begin(); it != new_triangles->end(); it++)
	{
		// Get or create the three new vertices
		for (i=0; i < 3; i++)
		{
			ver=(*it)->vertices[i];
			// avoid using vertices more than once
			iv=vertex_map.find(ver);
			if (iv == vertex_map.end())
			{
				// Create one new vertex
				v[i] = new Vertex( ver->x(), ver->y(), ver->z() );
				add_vertex(v[i]);
				shape_vertices->push_back(v[i]);
				// old vertex => new vertex
				vertex_map[ver] = v[i];
			}
			else
				// We found the vertex in our list
				v[i]=(*iv).second;
		}

		// Create one new triangle with the three vertices
		tri = new Triangle(v[0], v[1], v[2]);
		for (i=0; i < 3; i++)
			tri->set_text_coordinates( i, (*it)->get_text_s(i),
			(*it)->get_text_t(i) );
		add_triangle(tri);
		shape_triangles->push_back(tri);
	}

	shapes->push_back(new Shape(shape_triangles, 
		shape_vertices, texture_name));

	if (clear_map)
		vertex_map.clear();
}

void Mesh::add_mesh(Mesh *mesh)
{
	// adding shapes, triangles or edges automatically creates the vertices

	if (mesh != NULL)
		if (mesh->number_of_shapes() > 0)
		{
			for (int i=0; i < mesh->number_of_shapes(); i++)
				if (mesh->get_shape(i)->number_of_triangles() > 0)
					add_triangles(mesh->get_shape(i)->triangles,
					mesh->get_shape(i)->get_texture_name(), FALSE);
				else if (mesh->get_shape(i)->number_of_edges() > 0)
					add_edges(mesh->get_shape(i)->edges, FALSE);
				else if (mesh->get_shape(i)->number_of_vertices() > 0)
					add_vertices(mesh->get_shape(i)->vertices);

				vertex_map.clear();
		}
		else
		{
			if (mesh->number_of_triangles() > 0)
				add_triangles(mesh->triangles);
			else if (mesh->number_of_edges() > 0)
				add_edges(mesh->get_edges());
			else if (mesh->number_of_vertices() > 0)
				add_vertices(mesh->get_vertices());
		}
}

void Mesh::set_mesh(Mesh *mesh)
{
	model_centroid = mesh->model_centroid;
	model_scale = mesh->model_scale;

	clear();

	add_mesh(mesh);
}

int Mesh::read(FILE *f, int (*update_progress)(int pos),
			   void (*set_total)(int size))
{
	return 99;
}

void Mesh::write(FILE *f, const char *comment)
{
}

Vertex* Mesh::get_vertex(unsigned int name) const
{
	list<Vertex*>::iterator iv;

	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		if ((*iv)->name == name)
			return *iv;

	return NULL;
}

Edge* Mesh::get_edge(unsigned int name) const
{
	list<Edge*>::iterator ie;

	for (ie=edges->begin(); ie != edges->end(); ie++)
		if ((*ie)->name == name)
			return *ie;

	return NULL;
}

Triangle* Mesh::get_triangle(unsigned int name) const
{
	list<Triangle*>::iterator it;

	for (it=triangles->begin(); it != triangles->end(); it++)
		if ((*it)->name == name)
			return *it;

	return NULL;
}

int Mesh::type(FILE *f)
{
	char s1[21], s2[21];
	int g, end_of_line;
	float p;

	if (fscanf(f,"%20s %20s", s1, s2) != 2) // Read the first two strings
		return UNKNOWN_MESH; // Unknown file format

	end_of_line = fgetc(f) == '\n' ? TRUE : FALSE;

	rewind(f);

	// check geomview
	if (strcasecmp(s1,"OFF") == 0)
		return GEOMVIEW_MESH;

	// check ply
	if (strcasecmp(s1,"PLY") == 0)
		return PLY_MESH;

	// check vrml1
	else if (strcasecmp(s1,"#VRML") == 0 && strcasecmp(s2,"V1.0") == 0)
		return VRML1_MESH;

	// check vrml2
	else if (strcasecmp(s1,"#VRML") == 0 && strcasecmp(s2,"V2.0") == 0)
		return VRML2_MESH;

	// check pmesh
	else if (strcasecmp(s1,"#PMESH") == 0 || (strcasecmp(s1,"V") == 0 && sscanf(s2,"%f",&p) == 1 && !end_of_line))
		return P_MESH;

	// check gts
	else if (sscanf(s1,"%d",&g) == 1 && sscanf(s2,"%d",&g) == 1 && !end_of_line)
		return GTS_MESH;

	// check features
	else if (strcasecmp(s1,"#FEATURES") == 0)
		return FEATURES_MESH;

	// check mesh list
	else if (strcasecmp(s1,"#LIST") == 0)
		return LIST_MESH;

	// re-check gts with initial comments skipped
	// skip any preceeding GTS comment lines (like those produced by k3d)

	else {
		while (s1[0] == '#'){
			while ((fgetc(f) != '\n') && (!(feof(f)))) // Reads till end of the line
				;
			if (fscanf(f,"%20s %20s", s1, s2) != 2) // re-read the first two strings
				return UNKNOWN_MESH; // Unknown file format
		}
		end_of_line = fgetc(f) == '\n' ? TRUE : FALSE;
		if (sscanf(s1,"%d",&g) == 1 && sscanf(s2,"%d",&g) == 1 && !end_of_line){
			return GTS_MESH;
		}
	}

	return UNKNOWN_MESH; // Unknown file format
}

int Mesh::number_of_shapes(void) const
{
	return shapes->size();
}

list<Triangle*>* Mesh::get_triangles(void) const
{
	return triangles;
}

list<Vertex*>* Mesh::get_vertices(void) const
{
	return vertices;
}

list<Edge*>* Mesh::get_edges(void) const
{
	return edges;
}

Shape* Mesh::get_shape(unsigned int no) const
{
	if (no < shapes->size())
		return (*shapes)[no];

	return NULL;
}

list<Edge*>* Mesh::get_edges(list<Edge*> *es) const
{
	list<Edge*> *edge_list = new list<Edge*>;
	list<Edge*>::iterator ie1, ie2;

	for (ie1=es->begin(); ie1 != es->end(); ie1++)
		for (ie2=edges->begin(); ie2 != edges->end(); ie2++)
			if ((*ie2)->equal(*ie1))
				edge_list->push_back(*ie2);

	return edge_list;
}

Edge* Mesh::get_edge(map<pair<Vertex*,Vertex*>,Edge*> *edge_map, 
					 Vertex *v1, Vertex *v2)
{
	map<pair<Vertex*,Vertex*>,Edge*>::iterator ei;

	// search in map (hash table)
	// first try to get the edge
	ei=edge_map->find( pair<Vertex*,Vertex*> (v1, v2) );
	// second try to get the edge (reverse order)
	if (ei == edge_map->end())
		ei=edge_map->find( pair<Vertex*,Vertex*> (v2, v1) );
	if (ei == edge_map->end())
	{
		// create new edge
		Edge *e = new Edge(v1, v2);
		add_edge(e);

		// new hash entry
		(*edge_map)[ pair<Vertex*,Vertex*> (v1, v2) ] = e;
		return e;
	}
	else
		return (*ei).second;
}

void Mesh::create_edges(void)
{
	list<Triangle*>::iterator it;
	map<pair<Vertex*,Vertex*>,Edge*> edge_map;
	Edge *e;

	for (it=triangles->begin(); it != triangles->end(); it++)
	{
		// edge 1
		e = get_edge(&edge_map, (*it)->vertices[0], (*it)->vertices[1]);
		(*it)->edges[0]=e;   e->addTriangle(*it);

		// edge 2
		e = get_edge(&edge_map, (*it)->vertices[0], (*it)->vertices[2]);
		(*it)->edges[1]=e;   e->addTriangle(*it);

		// edge 3
		e = get_edge(&edge_map, (*it)->vertices[1], (*it)->vertices[2]);
		(*it)->edges[2]=e;   e->addTriangle(*it);
	}
}

float Mesh::average_triangle_size(void) const
{
	list<Triangle*>::iterator it;
	float size=0.0;

	for (it=triangles->begin(); it != triangles->end(); it++)
		size+=(*it)->perimeter();

	return size/tri_nr;
}

void Mesh::move_to_centre(void)
{
	model_centroid = get_centroid();
	model_centroid.negation();
	move(&model_centroid);
	model_centroid.negation();
}

MathVector Mesh::get_centroid(void) const
{
	list<Vertex*>::iterator iv;
	MathVector c;

	// find centroid
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		c.add((*iv)->math_data());
	c.scale(1.0/ver_nr);

	return c;
}

void Mesh::move(const MathVector *v)
{
	list<Triangle*>::iterator it;
	list<Vertex*>::iterator iv;

	// Move vertices and triangle centroids
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		(*iv)->move(v);
	for (it=triangles->begin(); it != triangles->end(); it++)
		(*it)->moveCentroid(v);
}

void Mesh::scale_into_normal_sphere(void)
{
	// Scaling mesh into +/- 1 sphere
	model_scale = sqrt(get_max_vertex_length());//可控制模型的大小
	scale( model_scale );
	model_scale = 1 / model_scale;
}

float Mesh::get_max_vertex_length(void) const
{
	list<Vertex*>::iterator iv;
	float length=0.0;

	// Finding the length
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		length = max( (*iv)->length(), length );

	return length;
}

void Mesh::scale(float scale)
{
	list<Triangle*>::iterator it;
	list<Edge*>::iterator ie;
	list<Vertex*>::iterator iv;

	// Scaling vertices
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		(*iv)->scale(scale);
	// Recalculate triangle centroid, size, perimeter and so on
	for (it=triangles->begin(); it != triangles->end(); it++)
		(*it)->calc_properties();
	// Recalculate edge centroid, length and so on
	for (ie=edges->begin(); ie != edges->end(); ie++)
		(*ie)->calc_properties();
}

void Mesh::calc_original_coordinates(const Vertex *v, Vertex *org) const
{
	org->set(v);
	org->scale(model_scale);
	org->move(&model_centroid);
}

void Mesh::clear_selection(void)
{
	selected_vertices.clear();
	selected_edges.clear();
	selected_triangles.clear();
}

Vertex* Mesh::select_vertex(unsigned int name)
{
	Vertex *v = get_vertex(name);

	if (v != NULL)
		selected_vertices.push_back(v);

	return v;
}

Edge* Mesh::select_edge(unsigned int name)
{
	Edge *e = get_edge(name);

	if (e != NULL)
		selected_edges.push_back(e);

	return e;
}

Triangle* Mesh::select_triangle(unsigned int name)
{
	Triangle *t = get_triangle(name);

	if (t != NULL)
		selected_triangles.push_back(t);

	return t;
}

void Mesh::negate_surface_normals(void)
{
	list<Triangle*>::iterator it;

	// negate all triangle surface normals
	for (it=triangles->begin(); it != triangles->end(); it++)
		(*it)->negate_normal();
}

void Mesh::remove_double_points(void)
{
	// the MathVector represents the x, y and z coordinates of the vertex
	map<MathVector, Vertex*> vertex_map;
	map<MathVector, Vertex*>::iterator i_map;
	list<Vertex*>::iterator i_list;
	list<Triangle*>::iterator it;
	list<Triangle*> *ver_triangles;
	list<Edge*>::iterator ie;
	list<Edge*> *ver_edges;

	for (i_list=vertices->begin(); i_list != vertices->end();)
	{
		// Check if we have the vertex already
		i_map=vertex_map.find( *(*i_list)->math_data() );
		if (i_map == vertex_map.end())
		{
			// Create a new entry
			vertex_map[ *(*i_list)->math_data() ] = *i_list;
			i_list++;
		}
		else
		{
			// change all references to the vertex
			ver_triangles=(*i_list)->get_triangles();
			for (it=ver_triangles->begin(); it != ver_triangles->end(); it++)
				(*it)->change_vertex(*i_list, (*i_map).second);
			ver_edges=(*i_list)->get_edges();
			for (ie=ver_edges->begin(); ie != ver_edges->end(); ie++)
				(*ie)->change_vertex(*i_list, (*i_map).second);

			// remove from shapes
			for (int i=0; i < number_of_shapes(); i++)
				get_shape(i)->change_vertex(*i_list, (*i_map).second);

			// delete vertex and remove from list
			delete *i_list;
			i_list=vertices->erase(i_list);
		}
	}

	ver_nr=vertices->size();
}

Vertex* Mesh::find_closed_point(const Vertex *v) const
{
	list<Vertex*>::iterator iv;
	MathVector con;
	Vertex *best = NULL;
	float best_length;

	best_length=MAXFLOAT;
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		MathVector::sub((*iv)->math_data(), v->math_data(), &con);
		if (con.length() < best_length)
		{
			best_length=con.length();
			best=*iv;
		}
	}

	return best;
}

void Mesh::write_gts_points(FILE *f) const
{
	int n;
	list<Vertex*>::iterator iv;

	// header
	fprintf(f,"%d 0 0\n", ver_nr);

	// vertex::number == vertex::name ???

	n=0;   // vertices
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		fprintf(f,"%f %f %f\n", (*iv)->x(), (*iv)->y(), (*iv)->z());
		n++;
		(*iv)->number=n;
	}
}

void Mesh::write_points(FILE *f) const
{
	list<Vertex*>::iterator iv;

	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		fprintf(f,"%f %f %f\n", (*iv)->x(), (*iv)->y(), (*iv)->z());
}
