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
#include<fstream>

Mesh::Mesh()
{
	edge_nr = ver_nr = tri_nr = 0;

	model_centroid.set_zero();
	model_scale = 1.0;
	the_min = 100;
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
	int n;
	list<Triangle*>::iterator it;
	list<Vertex*>::iterator iv;
	Vertex v;

	fprintf(f, "ply\n");
	fprintf(f, "format ascii 1.0\n");
	fprintf(f, "comment %s\n", comment);
	fprintf(f, "element vertex %i\n", ver_nr);
	fprintf(f, "property float x\n");
	fprintf(f, "property float y\n");
	fprintf(f, "property float z\n");

	fprintf(f, "property uchar red\n");
	fprintf(f, "property uchar green\n");
	fprintf(f, "property uchar blue\n");
	fprintf(f, "property uchar alpha\n");

	fprintf(f, "element face %i\n", tri_nr);
	fprintf(f, "property list uchar int vertex_indices\n");
	fprintf(f, "end_header\n");

	n=0;    // vertices
	//for (iv=vertices->begin(); iv != vertices->end(); iv++)
	//{
	//	/*calc_original_coordinates(*iv, &v);
	//	fprintf(f,"%f %f %f\n", v.x(), v.y(), v.z());*/
	//	fprintf(f,"%f %f %f\n", (*iv)->x(), (*iv)->y(), (*iv)->z());
	//	(*iv)->number=++n;
	//}

	for (iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		/*calc_original_coordinates(*iv, &v);
		fprintf(f,"%f %f %f\n", v.x(), v.y(), v.z());*/
		fprintf(f,"%f %f %f %d %d %d %d\n", (*iv)->math_newdata()->v[0], (*iv)->math_newdata()->v[1], (*iv)->math_newdata()->v[2],(*iv)->color[0],(*iv)->color[1],(*iv)->color[2],255);
		(*iv)->number=++n;
	}

	// triangles
	for (it=triangles->begin(); it != triangles->end(); it++)
		fprintf(f,"3 %d %d %d\n", ((*it)->vertices[0]->number - 1),
		((*it)->vertices[1]->number - 1), ((*it)->vertices[2]->number) - 1);
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

void Mesh::mymove_to_centre()
{
	list<Vertex*>::iterator iv;
	MathVector c;
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		c.add((*iv)->math_newdata());
	c.scale(1.0/ver_nr);
	c.negation();
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		(*iv)->newdata.add(&c);
}

MathVector Mesh::get_centroid(void) const
{
	list<Vertex*>::iterator iv;
	MathVector c;

	// find centroid
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
		c.add((*iv)->math_data());
	c.scale(1.0/ver_nr);
	printf("the center of model...:%f %f %f\n",c.v[0],c.v[1],c.v[2]);
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


void Mesh::initialize_delta()
{
	MathVector c;
	deltamesh=MatrixXf::Zero(vertices->size(),3);
	list<Vertex*>::iterator iv;
	list<Vertex*>::iterator ivv;
	int count=0;
	int row=0;
	fstream f("documents\\deltamesh.txt",ios::out);
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		for(ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
		{
			c.add((*ivv)->math_data());
			count++;
		}
		c.scale(1.0/count);
		count=0;
		c.sub((*iv)->math_data());
		c.scale(-1.0);
		for(int i=0;i<3;i++)
		{
			deltamesh(row,i)=c.v[i];
			f<<c.v[i]<<" ";
		}
		c.set_zero();
		row++;
	}
	f<<endl;
	f.close();
	fstream f2("documents\\delta.txt",ios::out);
	f2<<deltamesh<<endl;
	f2.close();
//随机丢失实验
	//int sum = vertices->size();
	//int lost = (int)(sum*0.1);
	//int rr;
	//vector<int> vi;
	//srand( (unsigned)time( NULL ) );
	//for(int i=0;i<lost;i++)
	//{
	//	rr = rand() % (sum+1);
	//	std::cout<<rr<<" ";
	//	vi.push_back(rr);
	//	/*deltamesh(rr,0) = 0.0;
	//	deltamesh(rr,1) = 0.0;
	//	deltamesh(rr,2) = 0.0;*/
	//}
	//cout<<endl;
	//sort(vi.begin(),vi.end());
	//cout<<"vi size...:"<<vi.size()<<endl;
	//list<int> li;
	//li.insert(li.begin(),vi.begin(),vi.end());
	//li.unique();
	//vi.clear();vi.insert(vi.begin(),li.begin(),li.end());
	//li.clear();
	//cout<<"vi size...:"<<vi.size()<<endl;
	//for(vector<int>::iterator it = vi.begin();it!=vi.end();it++)
	//{
	//	cout<<*it<<" ";
	//}
	//cout<<endl;
	//int ii =0;
	//int ccount=0;
	//RowVector3f rv;
	//fstream ff("documents\\chazhi.txt",ios::out);
	//for(iv=vertices->begin();iv!=vertices->end();iv++)
	//{
	//	if(ii<vi.size())
	//	{
	//		if(ccount==vi[ii])
	//		{	
	//			rv = deltamesh.row(ccount);
	//			ff<<rv(0)<<" "<<rv(1)<<" "<<rv(2)<<" ";
	//			deltamesh(ccount,0) = 0.0;
	//			deltamesh(ccount,1) = 0.0;
	//			deltamesh(ccount,2) = 0.0;
	//			for(list<Vertex*>::iterator ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
	//			{
	//				deltamesh.row(ccount) = deltamesh.row(ccount)+deltamesh.row((*ivv)->name-1);
	//			}
	//			deltamesh.row(ccount) = deltamesh.row(ccount)/(((*iv)->neighborvertex).size());
	//			ii++;
	//			rv = deltamesh.row(ccount);
	//			ff<<rv(0)<<" "<<rv(1)<<" "<<rv(2)<<endl;
	//		}
	//	}
	//	ccount++;
	//}
	//ff.close();

}

void Mesh::ini_ner_ver()
{
	list<Vertex*>::iterator iv;
	for(iv=vertices->begin();iv!=vertices->end();iv++)
	{
		(*iv)->get_neighborvertexs();
	}
}
void Mesh::quantizing_laplacian_coordinates(int bit)
{
	MathVector c;
	Vertex* current_vertex;
	cout<<"bit: "<<bit<<endl;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		c.set_zero();
		current_vertex = (*iv);
		for(list<Vertex*>::iterator ivv=current_vertex->neighborvertex.begin();ivv!=current_vertex->neighborvertex.end();ivv++)
		{
			c.add((*ivv)->math_data());
		}
		c.scale(1.0/current_vertex->neighborvertex.size());
		c.sub(current_vertex->math_data());
		c.negation();
		current_vertex->lapcoor.set(c.v[0],c.v[1],c.v[2]);

		/*if((*iv)->dye==0)
			vesq.push_back((*iv));*/
	}
	float the_max;
	float the_min;
	float chazhi;
	float max_error;
	for(int i=0; i<3; i++)
	{
		the_max=-100;
		the_min=100;
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			the_max = max(the_max,(*iv)->math_lapcoor()->v[i]);
			the_min = min(the_min,(*iv)->math_lapcoor()->v[i]);
		}
		chazhi = the_max - the_min;
		max_error = chazhi*pow(2.0,-bit);
		cout<<"max_error: "<<max_error<<endl;
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			(*iv)->math_quantized_lapcoor()->v[i] = floor(((*iv)->math_lapcoor()->v[i]-the_min)/max_error +0.5)*max_error +the_min;
		}
	}
	fstream f("documents\\quantized laplacian.txt",ios::out);
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		//f<<(*iv)->math_lapcoor()->v[0]<<" "<<(*iv)->math_lapcoor()->v[1]<<" "<<(*iv)->math_lapcoor()->v[2]<<" ";
		f<<(*iv)->math_quantized_lapcoor()->v[0]<<" "<<(*iv)->math_quantized_lapcoor()->v[1]<<" "<<(*iv)->math_quantized_lapcoor()->v[2]<<endl;
	}
	f.close();
}

void Mesh::initialize_tripletList_and_deltamesh()
{
	deltamesh=MatrixXf::Zero(vertices->size(),3);
	int i,j;
	tripletList.clear();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		i = (*iv)->name-1;
		tripletList.push_back(T(i, i, (*iv)->neighborvertex.size()));
		for(int ii=0;ii<3;ii++)
		{
			deltamesh(i,ii)=((*iv)->math_quantized_lapcoor()->v[ii])*(*iv)->neighborvertex.size();
		}
		for(list<Vertex*>::iterator ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
		{
			j = (*ivv)->name-1;
			tripletList.push_back(T(i, j, -1.0));
		}
	}
	/*srand( (unsigned)time( NULL ) );
	int rr = rand()%vertices->size();
	*list<Vertex*>::iterator iv=vertices->begin();
	first_anchor = (*iv);*/
	double leng = -100;
	double ll;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		ll = (*iv)->math_quantized_lapcoor()->length();
		if(ll > leng)
		{
			leng = ll;
			first_anchor = (*iv);
		}
	}
	cout<<"leng: "<<leng<<endl;
	/*list<Vertex*>::iterator iv=vertices->begin();
	first_anchor = (*iv);*/
	/*srand( (unsigned)time( NULL ) );
	int rr = rand()%vertices->size();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		if(((*iv)->name-1)==rr)
		{
			first_anchor = (*iv);
			break;
		}
	}*/
}

void Mesh::iterator_soving_least_square(int anchor)
{
	SpMat slaplacian((vertices->size()+anchor),vertices->size());
	MatrixXf delta = MatrixXf::Zero(vertices->size()+anchor,3);
	vector<T> temptripletList(tripletList);
	//cout<<"test..temptripletList size:"<<temptripletList.size()<<endl;
	for(int i=0;i<vertices->size();i++)
	{
		delta.row(i) = deltamesh.row(i);
	}
	int appended = vertices->size();
	int j;
	for(vector<Vertex*>::size_type st=0;st<vesq.size();st++)
	{
		j = vesq[st]->name-1;
		temptripletList.push_back(T(appended, j, 1));
		for(int i=0;i<3;i++)
		{
			delta(appended,i)=vesq[st]->math_data()->v[i];
		}
		appended++;
	}
	slaplacian.setFromTriplets(temptripletList.begin(), temptripletList.end());
	//soving
	SpMat A = slaplacian.transpose()*slaplacian;
	SparseLU<SparseMatrix<float>, COLAMDOrdering<int> > lu;
	lu.analyzePattern(A);
	lu.factorize(A);
	MatrixXf deltaa = slaplacian.transpose()*delta;
	MatrixXf result = lu.solve(deltaa);

	temptripletList.clear();
	//save
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->newdata.v[0] = (result.col(0))((*iv)->name-1);
		(*iv)->newdata.v[1] = (result.col(1))((*iv)->name-1);
		(*iv)->newdata.v[2] = (result.col(2))((*iv)->name-1);
	}

}

void Mesh::iterator_compute_number(int num)//将锚点增加到多少个
{
	double temp;
	for(int i=0;i<num;i++)
	{
		iterator_soving_least_square(vesq.size());
		visual_quality_measure_for_greedy();
		//cout<<"误差值： "<<temp<<endl;
		/*if(i%10==9)
			cout<<"...................."<<endl;*/
	}
	//cout<<"最小值： "<<the_min<<endl;
}

void Mesh::replace_better_anchor(FILE *f,int num)
{
	cout<<"=========================================="<<endl;
	double temp;
	vector<Vertex*> aaa;
	double xx = 100;
	int count = 0;
	srand( (unsigned)time( NULL ) );
	for(int i=0;i<num;i++)
	{
		//sort(vesq.begin(),vesq.end(),Sqlarge);
		vesq.erase(vesq.begin()+(rand()%vesq.size()));
		iterator_soving_least_square(vesq.size());
		temp = visual_quality_measure(0.5);
		if(temp<xx)
			xx = temp;
		//cout<<"误差值： "<<temp<<endl;
		if(temp<the_min)
		{
			count++;
			aaa.clear();
			aaa.insert(aaa.begin(),vesq.begin(),vesq.end());
		}
		if(i%10==9)
			cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
	}
	/*
	cout<<"最小值： "<<the_min<<endl;
	cout<<"anchor： "<<vesq.size()-1<<endl;
	cout<<"hit percentage: "<<count*1.0/num<<endl;
	cout<<"xx： "<<xx<<endl;
	cout<<"chazhi： "<<the_min-xx<<endl;
	cout<<"percentage： "<<(the_min-xx)/the_min<<endl;*/
	/*fprintf(f,"Evis: %f\n",the_min);
	fprintf(f,"anchor： %d\n",vesq.size()-1);
	fprintf(f,"hit percentage: %f\n",count*1.0/num);
	fprintf(f,"xx： %f\n",xx);
	fprintf(f,"chazhi： %f\n",the_min-xx);
	fprintf(f,"percentage： %f\n",(the_min-xx)/the_min);
	fprintf(f,"=======================================\n");*/
	fprintf(f,"%d %f %f %f %f %f\n",vesq.size()-1, the_min, count*1.0/num, xx, the_min-xx, (the_min-xx)/the_min);
	//for(vector<Vertex*>::size_type sy =0;sy<aaa.size();sy++)
	//{
	//	fprintf(f,"%d ",aaa[sy]->name-1);
	//}
	//fprintf(f,"\n");
	//for(vector<Vertex*>::size_type sy =0;sy<aaa.size();sy++)
	//{
	//	fprintf(f,"%f ",aaa[sy]->math_quantized_lapcoor()->length());
	//}
	//fprintf(f,"\n");
	//计算选出的最好锚点的重构模型
	vesq.clear();
	vesq.insert(vesq.begin(),aaa.begin(),aaa.end());
	iterator_soving_least_square(vesq.size());
	ccccc();
}
void Mesh::ccccc()
{
	int color_emum[4][3]={{255,235,205},{127,255,0},{218,112,214},{255,0,0}};
	//把所有点染色
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->color[0] = color_emum[0][0];
		(*iv)->color[1] = color_emum[0][1];
		(*iv)->color[2] = color_emum[0][2];
	}
	//对锚点染色
	for(vector<Vertex*>::size_type sy = 0; sy<vesq.size()-1; sy++)
	{
		vesq[sy]->color[0] = color_emum[3][0];
		vesq[sy]->color[1] = color_emum[3][1];
		vesq[sy]->color[2] = color_emum[3][2];
	}
}

void Mesh::BFS_Graph(Vertex *anchor, int i)
{
	
	anchor->to_anthor_length = 0;//设置为anchor
	anchor->is_visited = true;
	queue<Vertex*> q1;
	vector<Vertex*> temp;
	q1.push(anchor);
	int count = 1;
	Vertex *te;
	Vertex *max_length;
	//max_length = anchor;
	while(!q1.empty())
	{
		te = q1.front();
		q1.pop();
		for(list<Vertex*>::iterator it = te->neighborvertex.begin(); it != te->neighborvertex.end();it++)
		{
			if(!(*it)->is_visited)//未被访问
			{
				(*it)->is_visited = true;
				if((*it)->to_anthor_length > count)
				{
					(*it)->to_anthor_length = count;
					/*if(max_length->to_anthor_length < count)
					{
						max_length = (*it);
					}*/
				}
				temp.push_back((*it));
			}
		}
		if(q1.empty())
		{
			for(vector<Vertex*>::size_type st =0;st<temp.size();st++)
			{
				q1.push(temp[st]);
			}
			temp.clear();
			count++;
		}
	}
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->is_visited = false;
	}
	sort(ver.begin(),ver.end(),Sqlarge);
	max_length = ver[0];
	
	//cout<<"当前topo最大距离的最小值: "<<max_length->to_anthor_length<<endl;
	if(max_length->to_anthor_length > i)
	{
		BFS_Graph(max_length,i);
	}
	
	//if(i>0)
	//{
	//	i--;
	//	BFS_Graph(max_length,i);
	//}
}

void Mesh::test_anchor(int bit, fstream &fs, string &ss)
{
	FILE *f;
	vector<int> anchorsize;
	//fstream f("documents\\mlength.txt",ios::out);
	string st(ss);
	quantizing_laplacian_coordinates(bit);//量化
	initialize_tripletList_and_deltamesh();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		ver.push_back((*iv));
		(*iv)->to_anthor_length = 99999;
	}
	for(int i=24;i>=5;i--)
	{
		BFS_Graph(first_anchor,i);
		vesq.clear();
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			if((*iv)->to_anthor_length == 0)
			{
				vesq.push_back((*iv));
			}
		}
		anchorsize.push_back(vesq.size());
		iterator_soving_least_square(vesq.size());
		//visual_quality_measure(0.5);
		hausdorff_distance_1();
		ccccc();
		st.append(convertToString(i));//int转string
		st.append(".ply");
		if((f=fopen(st.c_str(),"w"))==NULL)
		{
			exit(0);
		}
		write(f, "VCGLIB generated");
		fclose(f);
		st =ss;
	}
	for(vector<int>::size_type st=0;st<anchorsize.size();st++)
	{
		fs<<cevis[st]<<" "<<anchorsize[st]<<endl;
	}
	fs<<" "<<" "<<" "<<endl;
	ver.clear();
	anchorsize.clear();
	cevis.clear();
	cout<<"running............"<<endl;
	//f.close();
	/*BFS_Graph(first_anchor,10);
	vesq.clear();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		if((*iv)->to_anthor_length == 0)
		{
			vesq.push_back((*iv));
		}
	}
	cout<<"vesq.size: "<<vesq.size()<<endl;
	iterator_soving_least_square(vesq.size());
	visual_quality_measure(0.5);
	ccccc();*/
	
}

void Mesh::test_anchor1()
{
	quantizing_laplacian_coordinates(8);
	initialize_tripletList_and_deltamesh();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		ver.push_back((*iv));
	}
	BFS_Graph(first_anchor,6);
	vesq.clear();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		if((*iv)->to_anthor_length == 0)
		{
			vesq.push_back((*iv));
		}
	}
	iterator_soving_least_square(vesq.size());
	//Hausdorff_distance();
	hausdorff_distance_1();
	ccccc();
}

void Mesh::test_anchor2()
{
	unsigned long dwStart;
	unsigned long dwEnd;
	vector<unsigned long> ul;
	vector<int> anchorsize;
	quantizing_laplacian_coordinates(5);
	initialize_tripletList_and_deltamesh();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)	//被用作BFS排序，把离锚点最远的点设为锚点
	{
		ver.push_back((*iv));
	}
	dwStart = GetTickCount();
	for(int i=0;i<35;i++)
	{
		//dwStart = GetTickCount();
		if(i==0)
			BFS_Graph(first_anchor,9);
		else
			BFS_Graph(first_anchor,10);
		vesq.clear();
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			if((*iv)->to_anthor_length == 0)
			{
				vesq.push_back((*iv));
			}
		}
		anchorsize.push_back(vesq.size());
		iterator_soving_least_square(vesq.size());
		visual_quality_measure(0.5);
		dwEnd = GetTickCount();
		ul.push_back(dwEnd-dwStart);
	}
	//dwEnd = GetTickCount();
	printf("elapsed:%d\n", dwEnd-dwStart);
	fstream fs("documents\\mmm.txt",ios::out);
	for(vector<int>::size_type st=0;st<anchorsize.size();st++)
	{
		fs<<anchorsize[st]<<" "<<cevis[st]<<" "<<ul[st]<<endl;
	}
}

void Mesh::test_anchor3()
{	
	unsigned long dwStart;
	unsigned long dwEnd;
	vector<unsigned long> ul;
	vector<int> anchorsize;
	quantizing_laplacian_coordinates(5);
	initialize_tripletList_and_deltamesh();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)	//被用作BFS排序，把离锚点最远的点设为锚点
	{
		ver.push_back((*iv));
	}
	dwStart = GetTickCount();
	BFS_Graph(first_anchor,9);
	dwEnd = GetTickCount();
	vesq.clear();
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		if((*iv)->to_anthor_length == 0)
		{
			vesq.push_back((*iv));
		}
	}
	iterator_soving_least_square(vesq.size());
	visual_quality_measure(0.5);
	fstream fs("documents\\mmm.txt",ios::out);
	/*fs<<"times: "<<dwEnd-dwStart<<endl;
	fs<<"num anchors: "<<vesq.size()<<endl;
	fs<<"Mq: "<<cevis[0]<<" Sq: "<<cevis[1]<<endl;*/
	fs<<vesq.size()<<" "<<dwEnd-dwStart<<" "<<cevis[0]<<" "<<cevis[1]<<" ";
	//下面用Greedy选锚点
	int num = vesq.size()-1;
	vesq.clear();
	vesq.push_back(first_anchor);
	//打点
	dwStart = GetTickCount();
	iterator_compute_number(num);//将锚点增加到多少个
	dwEnd = GetTickCount();
	visual_quality_measure(0.5);
	fs<<dwEnd-dwStart<<" "<<cevis[2]<<" "<<cevis[3]<<endl;
	fs.close();
	/*for(int i=24;i>=5;i--)
	{
		BFS_Graph(first_anchor,i);
		vesq.clear();
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			if((*iv)->to_anthor_length == 0)
			{
				vesq.push_back((*iv));
			}
		}
		anchorsize.push_back(vesq.size());
		iterator_soving_least_square(vesq.size());
		visual_quality_measure(0.5);
		dwEnd = GetTickCount();
		ul.push_back(dwEnd-dwStart);
	}*/
	/*fstream fs("documents\\mmm.txt",ios::out);
	for(vector<int>::size_type st=0;st<anchorsize.size();st++)
	{
	fs<<anchorsize[st]<<" "<<cevis[st]<<" "<<ul[st]<<endl;
	}*/
}

void Mesh::statistics_data(FILE *f,int bit)
{
	int anchor[1] = {60};//各个锚点//5,10,15,20,25,30,35,40,45,50,55,60  10,20,30,40,50,60,70,80,90
	fprintf(f,"%s %d\n","bits: ",bit);
	quantizing_laplacian_coordinates(bit);//量化
	initialize_tripletList_and_deltamesh();
	the_min = 100;
	for(int i=0;i<1;i++)
	{
		cout<<"anchor: "<<anchor[i]<<endl;
		/*srand( (unsigned)time( NULL ) );
		int rr = rand()%vertices->size();
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			if(((*iv)->name-1)==rr)
			{
				first_anchor = (*iv);
				break;
			}
		}*/
		vesq.clear();
		vesq.push_back(first_anchor);//选好第一个锚点
		iterator_compute_number(anchor[i]);
		/*	ccccc();
		FILE *ff;
		if((ff=fopen("documents\\horse4243before.ply","w"))==NULL)
		{
		exit(0);	
		}
		write(ff, "VCGLIB generated");
		fclose(ff);*/
		/*	for(vector<Vertex*>::size_type sy =0;sy<vesq.size();sy++)
		{
		fprintf(f,"%d ",vesq[sy]->name-1);
		}
		fprintf(f,"\n");
		for(vector<Vertex*>::size_type sy =0;sy<vesq.size();sy++)
		{
		fprintf(f,"%f ",vesq[sy]->math_quantized_lapcoor()->length());
		}
		fprintf(f,"\n");*/
		//replace_better_anchor(f,100);
	}
	fstream ff("documents\\cevis.txt",ios::out);
	for(vector<double>::size_type st=0;st<cevis.size();st++)
	{
		ff<<cevis[st++]<<" "<<cevis[st]<<endl;
	}
	ff.close();
}

void Mesh::hausdorff_distance_1()
{
	Vertex *current_ver;
	Vertex *V;
	double temp;
	double foreminer;
	double backminer;
	double maxest = -1;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		current_ver = (*iv);
		foreminer = MathVector::length(current_ver->math_data(),current_ver->math_newdata());
		for(list<Vertex*>::iterator ivv = current_ver->neighborvertex.begin(); ivv != current_ver->neighborvertex.end();ivv++)
		{
			temp = MathVector::length(current_ver->math_newdata(),(*ivv)->math_data());
			if(foreminer > temp)
				foreminer = temp;
		}
		if(maxest < foreminer)
		{
			maxest = foreminer;
			V = current_ver;
		}
	}
	//cout<<"hausdorff distance: "<<maxest<<endl;
	cevis.push_back(maxest);
	//double dtemp;
	//double ddd = 10;
	//MathVector c;
	//c.set(V->math_newdata());
	//c.sub(V->math_data());
	//cout<<"c length: "<<c.length()<<endl;
	//for(list<Triangle*>::iterator it = V->get_triangles()->begin(); it != V->get_triangles()->end(); it++)
	//{
	//	dtemp = abs(MathVector::dot_product((*it)->get_surface_normal(),&c))/(*it)->get_surface_normal()->length();
	//	cout<<"dtemp: "<<dtemp<<endl;
	//	if(ddd > dtemp)
	//		ddd = dtemp;
	//}
	//cout<<"ddd: "<<ddd<<endl;
}

void Mesh::Hausdorff_distance()
{
	Triangle* current_tri;
	double temp;
	double miner;
	double forward_maxer;
	double backward_maxer;
	double hausdorff =0;
	double hausdorffback = 0;
	double the_maxest_forward = -1;
	double the_maxest_backward = -1;
	fstream of("documents\\hausdorff distance.txt",ios::out);
	of.seekp(ios::beg);
	for(list<Triangle*>::iterator it = triangles->begin();it != triangles->end();it++)
	{
		current_tri = (*it);
		forward_maxer = -1;
		for(int i=0;i<3;i++)	//找最大值
		{
			miner = 100;
			for(int j=0;j<3;j++)	//找最小值
			{
				temp = MathVector::length(current_tri->vertices[i]->math_data(),current_tri->vertices[j]->math_newdata());
				if(miner > temp)
				{
					miner = temp;
				}
			}
			if(forward_maxer < miner)
			{
				forward_maxer = miner;
			}
		}
		//......................................................
		backward_maxer = -1;
		for(int i=0;i<3;i++)	//找最大值
		{
			miner = 100;
			for(int j=0;j<3;j++)	//找最小值
			{
				temp = MathVector::length(current_tri->vertices[i]->math_newdata(),current_tri->vertices[j]->math_data());
				if(miner > temp)
				{
					miner = temp;
				}
			}
			if(backward_maxer < miner)
			{
				backward_maxer = miner;
			}
		}
		of<<forward_maxer<<" "<<backward_maxer<<endl;
		if(the_maxest_forward < forward_maxer)
			the_maxest_forward = forward_maxer;
		if(the_maxest_backward < backward_maxer)
			the_maxest_backward = backward_maxer;
		hausdorff += forward_maxer;
		hausdorffback += backward_maxer;
	}
	of.close();
	cout<<"the_maxest_forward: "<<the_maxest_forward<<endl;
	cout<<"the_maxest_backward "<<the_maxest_backward<<endl;
	cout<<"forward distance mean: "<<hausdorff/triangles->size()<<endl;
	cout<<"backward distance mean: "<<hausdorffback/triangles->size()<<endl;
}

void Mesh::visual_quality_measure_for_greedy()
{
	double Mq = 0;
	double Sq = 0;
	double temp;
	double maxerror = -100;
	Vertex* current_v;

	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		temp = MathVector::length2((*iv)->math_data(),(*iv)->math_newdata());
		(*iv)->Mq = temp;

		temp = (*iv)->geometric_laplacian();
		(*iv)->Sq = temp;

	}
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		temp = (*iv)->Mq;
		if(temp>maxerror)
		{
			maxerror = temp;
			current_v = (*iv);
		}
	}
	vesq.push_back(current_v);
}

double Mesh::visual_quality_measure(float a)
{
	double Mq = 0;
	double Sq = 0;
	double temp;
	double Evis;
	double maxerror = -100;
	Vertex* current_v;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		temp = MathVector::length2((*iv)->math_data(),(*iv)->math_newdata());
		(*iv)->Mq = temp;
		Mq += temp;

		temp = (*iv)->geometric_laplacian();
		(*iv)->Sq = temp;
		Sq += temp;

	}
	Mq = sqrt(Mq);
	Sq = sqrt(Sq);
	cevis.push_back(Mq);
	cevis.push_back(Sq);
	Evis = a*Mq + (1-a)*Sq;
	//cout<<"Mq: "<<Mq<<" Sq: "<<Sq<<" Evis: "<<Evis<<endl;
	//for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	//{
	//	temp = (*iv)->Mq*a + (*iv)->Sq*(1-a);
	//	if(temp>maxerror)
	//	{
	//		maxerror = temp;
	//		current_v = (*iv);
	//	}
	//}
	//vesq.push_back(current_v);
	return Evis;
}

void Mesh::solve_invertible_linear_equations()
{
	set<int> s;
	MathVector c;
	float scale = 1.0;
	int color_emum[8][3]={{255,97,0},{127,255,0},{218,112,214},{128,42,42},{8,46,84},{255,99,71},{65,105,225},{48,128,20}};
	set<int>::iterator looking;
	list<Vertex*>::iterator iv;
	list<Vertex*>::iterator ivv;
	typedef Eigen::Triplet<float> T;
	std::vector<T> tripletList;
	Vertex* current_vertex;
	int i,j;
	int row=0;
	SpMat slaplacian(vertices->size(),vertices->size());
	MatrixXf delta = MatrixXf::Zero(vertices->size(),3);
	for(iv = seeds.begin();iv != seeds.end();iv++)
	{
		s.insert((*iv)->name -1);
	}
	cout<<"set size:"<<s.size()<<endl;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		c.set_zero();
		current_vertex = (*iv);
		for(list<Vertex*>::iterator ivv=current_vertex->neighborvertex.begin();ivv!=current_vertex->neighborvertex.end();ivv++)
		{
			c.add((*ivv)->math_data());
		}
		c.scale(1.0/current_vertex->neighborvertex.size());
		c.sub(current_vertex->math_data());
		c.negation();
		current_vertex->lapcoor.set(c.v[0],c.v[1],c.v[2]);
	}

	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		i = (*iv)->name-1;
		looking = s.find(i);
		if(looking != s.end())	//对anchor点的处理
		{
			tripletList.push_back(T(i, i, scale));
			for(int i=0;i<3;i++)
			{
				delta(row,i)=(*iv)->math_data()->v[i]*scale;
			}
		}
		else
		{
			tripletList.push_back(T(i, i, (*iv)->neighborvertex.size()));
			for(int i=0;i<3;i++)
			{
				delta(row,i)=((*iv)->math_lapcoor()->v[i])*(*iv)->neighborvertex.size();
			}

			for(list<Vertex*>::iterator ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
			{
				j = (*ivv)->name-1;
				looking = s.find(j);
				if(looking == s.end())//没找到
				{
					tripletList.push_back(T(i, j, -1.0));
				}
			}
		}
		row++;
	}
	slaplacian.setFromTriplets(tripletList.begin(), tripletList.end());
	// Solving:
	Eigen::SimplicialCholesky<SpMat> chol(slaplacian);  // performs a Cholesky factorization of A
	MatrixXf result = chol.solve(delta);

	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->newdata.v[0] = (result.col(0))((*iv)->name-1);
		(*iv)->newdata.v[1] = (result.col(1))((*iv)->name-1);
		(*iv)->newdata.v[2] = (result.col(2))((*iv)->name-1);
	}
}


void Mesh::solve_sparse_linear_equations()
{
	typedef Eigen::Triplet<float> T;
	std::vector<T> tripletList;
	SpMat slaplacian((vertices->size()),vertices->size());
	MatrixXf delta = MatrixXf::Zero(vertices->size(),3);
	int i,j;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		i = (*iv)->name-1;
		tripletList.push_back(T(i, i, (*iv)->neighborvertex.size()));
		for(int ii=0;ii<3;ii++)
		{
			delta(i,ii)=((*iv)->math_quantized_lapcoor()->v[ii])*(*iv)->neighborvertex.size();
		}
		for(list<Vertex*>::iterator ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
		{
			j = (*ivv)->name-1;
			tripletList.push_back(T(i, j, -1.0));
		}
	}
	slaplacian.setFromTriplets(tripletList.begin(), tripletList.end());
	// Solving:
	Eigen::SimplicialCholesky<SpMat> chol(slaplacian);  // performs a Cholesky factorization of A
	MatrixXf result = chol.solve(delta);
	
	//保存求解的坐标值
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->newdata.v[0] = (result.col(0))((*iv)->name-1);
		(*iv)->newdata.v[1] = (result.col(1))((*iv)->name-1);
		(*iv)->newdata.v[2] = (result.col(2))((*iv)->name-1);
	}

	//move
	mymove_to_centre();

}

void Mesh::lookfor_seeds(int n)
{
	list<Vertex*>::iterator iv, it;
	Vertex *fir_seed = NULL;
	iv=vertices->begin();
	fir_seed=(*iv);
	iv++;
	for(; iv!=vertices->end(); iv++)
	{
		if(fir_seed->length()<(*iv)->length())
		{
			fir_seed=(*iv);
		}
	}
	seeds.push_back(fir_seed);//找到第一个种子

	while(--n)
	{
		double longer=0.0;
		Vertex *current_seed = NULL;
		for(iv=vertices->begin();iv!=vertices->end();iv++)
		{
			double sum_length=0.0;
			for(list<Vertex*>::iterator is = seeds.begin(); is != seeds.end(); is++)
			{
				sum_length += MathVector::length((*iv)->math_data(), (*is)->math_data());
			}

			if(longer<sum_length)
			{
				it=find(seeds.begin(), seeds.end(), (*iv));
				if(it!=seeds.end())
				{
					continue;
				}
				else
				{
					longer = sum_length;
					current_seed = (*iv);
				}
			}
		}
		seeds.push_back(current_seed);
	}//while循环结束
	cout<<"种子个数："<<seeds.size()<<endl;
}

void Mesh::segment_graph(void)
{
	queue<Vertex*> *q = new queue<Vertex*>[seeds.size()];
	list<Vertex*> *part = new list<Vertex*>[seeds.size()];
	int i = 0;
	for(list<Vertex*>::iterator iv = seeds.begin();iv != seeds.end();iv++)//初始化
	{
		q[i].push((*iv));
		part[i].push_back((*iv));
		(*iv)->is_segment_traversed=true;//被访问设置
		i++;
	}

	int flag=0;
	for(int i=0;i<seeds.size();i++)
	{
		flag=(flag||(q[i].size()));
		if(flag)
		{
			break;
		}
	}

	while(flag)
	{
		vector<int> vv;
		for(int j=0;j<seeds.size();j++)
		{
			if(q[j].size()!=0)
			{
				vv.push_back(j);
			}
		}

		vector<int>::size_type iv =0;
		int min_part = part[vv[iv]].size();
		int k=vv[iv];
		for(iv=1;iv!=vv.size();iv++)
		{
			if(min_part>part[vv[iv]].size())
			{
				min_part = part[vv[iv]].size();
				k = vv[iv];
			}
		}

		if((q[k].size() != 0))
		{
			Vertex *v=q[k].front();
			q[k].pop();
			for(list<Vertex*>::iterator iv=v->neighborvertex.begin();iv!=v->neighborvertex.end();iv++)
			{
				if((*iv)->is_segment_traversed==false)
				{
					q[k].push((*iv));
					part[k].push_back((*iv));
					(*iv)->is_segment_traversed=true;					
				}
			}
		}
		flag=0;
		for(int i=0;i<seeds.size();i++)//更改循环条件
		{
			flag=(flag||(q[i].size()));
			if(flag)
			{
				break;
			}
		}
	}//while循环结束
	printf("每一块的size...: ");
	int count_part=0;
	for(int i=0;i<seeds.size();i++)
	{
		printf("%d ",part[i].size());
		count_part+=part[i].size();
		superlist.push_back(part[i]);
	}
	printf("\n");
	printf("%s %d\n","总共的大小：",count_part);
}

void Mesh::ini_segmented_ner_ver()
{
	list<Vertex*> *part_list;
	vector<Vertex*> temp;
	vector<Vertex*>::iterator result;
	int count;
	printf("%s %d\n","superlist 的大小：",superlist.size());
	for(vector<list<Vertex*>>::size_type liv = 0; liv != superlist.size();liv++)
	{
		part_list = &superlist[liv];
		temp.clear();
		count=1;
		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			temp.push_back((*it));
			(*it)->blockid = count++;
		}

		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			for(list<Vertex*>::iterator nei_it = (*it)->neighborvertex.begin();nei_it != (*it)->neighborvertex.end();nei_it++)
			{
				result = find(temp.begin(),temp.end(),(*nei_it));
				if(result != temp.end())//找到
				{
					(*it)->segmented_neighborvertex.push_back((*nei_it));
				}
			}
		}
	}
}

void Mesh::ini_segmented_nei2_ver()
{
	list<Vertex*> *part_list;
	Vertex* current_vertex;
	list<Vertex*>::iterator looking;
	for(vector<list<Vertex*>>::size_type liv = 0; liv != superlist.size();liv++)
	{
		part_list = &superlist[liv];
		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			current_vertex = (*it);
			for(list<Vertex*>::iterator nei = current_vertex->segmented_neighborvertex.begin(); nei != current_vertex->segmented_neighborvertex.end(); nei++)
			{
				current_vertex->segmented_neighborvertex2.push_back((*nei));
			}

			for(list<Vertex*>::iterator nei = current_vertex->segmented_neighborvertex.begin(); nei != current_vertex->segmented_neighborvertex.end(); nei++)
			{
				for(list<Vertex*>::iterator nei2 = (*nei)->segmented_neighborvertex.begin(); nei2 != (*nei)->segmented_neighborvertex.end();nei2++)
				{
					if((*nei2)->name != current_vertex->name)
					{
						looking = find(current_vertex->segmented_neighborvertex2.begin(),current_vertex->segmented_neighborvertex2.end(),(*nei2));
						if(looking == current_vertex->segmented_neighborvertex2.end())//not find
						{
							current_vertex->segmented_neighborvertex2.push_back((*nei2));
						}
					}
				}
			}

		}
	}
}

void Mesh::greedy_packet()
{
	list<Vertex*> *part_list;
	int *dye_count, *bit_color;
	int packet_num;
	int min_count,colo;
	Vertex* current_vertex;
	vector<int> test_packetsise;
	for(vector<list<Vertex*>>::size_type liv = 0; liv != superlist.size();liv++)
	{
		part_list = &superlist[liv];
		packet_num = part_list->size()/120 + 1;
		dye_count = new int[packet_num];//可着色颜色个数
		bit_color = new int[packet_num];
		memset(dye_count,0,sizeof(dye_count)*(packet_num));
		memset(bit_color,0,sizeof(bit_color)*(packet_num));

		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			current_vertex = (*it);
			for(list<Vertex*>::iterator nei = current_vertex->segmented_neighborvertex2.begin(); nei != current_vertex->segmented_neighborvertex2.end(); nei++)
			{
				if((*nei)->dye != -1)//表示已着色
					bit_color[(*nei)->dye]=1;
			}
			int i;
			for(i=0;i<packet_num;i++)//找出未被着过的一种颜色
			{
				if(bit_color[i]==0)
				{
					min_count=dye_count[i];
					colo = i;
					break;
				}
			}
			for(int j=i+1;j<packet_num;j++)
			{
				if(min_count > dye_count[j])
				{
					min_count=dye_count[j];
					colo = j;
				}
			}
			current_vertex->dye = colo;
			dye_count[colo]++;
			memset(bit_color,0,sizeof(bit_color)*(packet_num));
		}
		for(int i =0;i<packet_num;i++)
		{
			li = new list<Vertex*>;
			for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
			{
				if((*it)->dye==i)
					li->push_back((*it));
			}
			supervector.push_back(li);
			test_packetsise.push_back(dye_count[i]);
		}
		delete[] dye_count;
		delete[] bit_color;
	}//最外层的for

	fstream f("documents\\packetsize.txt",ios::out);
	int count = 0;
	for(vector<int>::size_type sy=0; sy<test_packetsise.size();sy++)
	{
		f<<test_packetsise[sy]<<" ";
		count += test_packetsise[sy];
		if(sy%5==4)
			f<<endl;
	}
	f<<count<<" "<<vertices->size()<<endl;
	f<<supervector.size();
	f.close();
}

void Mesh::lost_laplacian_coordinates()
{
	vector<int> lost_packet;
	unsigned _time = (unsigned)time( NULL );
	srand( _time );//srand()函数产生一个以当前时间开始的随机种子
	int percent=10;
	int range_max = supervector.size();
	percent = range_max/100.0*percent;
	for(int i =0;i<percent;i++)
	{
		int u = (double)rand() / (RAND_MAX + 1) * (range_max);
		lost_packet.push_back(u);
	}
	list<Vertex*> *part;
	for(vector<int>::size_type is =0; is <lost_packet.size();is++)
	{
		//cout<<lost_packet[is]<<" ";
		part = supervector[lost_packet[is]];
		for(list<Vertex*>::iterator it = part->begin(); it != part->end();it++)
		{
			(*it)->is_laplacian_lost = true; 
		}
	}
	cout<<lost_packet.size()<<endl;
}

void Mesh::solving_least_squares(int anchor)
{
	MathVector c;
	vector<Vertex*> seedsq;
	list<Vertex*>::iterator looking;
	int color_emum[8][3]={{255,97,0},{127,255,0},{218,112,214},{128,42,42},{8,46,84},{255,99,71},{65,105,225},{48,128,20}};
	typedef Eigen::Triplet<float> T;
	std::vector<T> tripletList;
	SpMat slaplacian((vertices->size()+anchor),vertices->size());
	MatrixXf delta = MatrixXf::Zero(vertices->size()+anchor,3);
	Vertex* current_vertex;
	int i,j;
	int row=0;
	cout<<"anchor: "<<anchor<<endl;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		i = (*iv)->name-1;
		tripletList.push_back(T(i, i, (*iv)->neighborvertex.size()));
		for(int ii=0;ii<3;ii++)
		{
			delta(row,ii)=((*iv)->math_quantized_lapcoor()->v[ii])*(*iv)->neighborvertex.size();
		}
		for(list<Vertex*>::iterator ivv=((*iv)->neighborvertex).begin();ivv!=((*iv)->neighborvertex).end();ivv++)
		{
			j = (*ivv)->name-1;
			tripletList.push_back(T(i, j, -1.0));
		}
		row++;
	}
	int appended = vertices->size();
	for(vector<Vertex*>::size_type st=0;st<anchor;st++)
	{
		j = vesq[st]->name-1;
		tripletList.push_back(T(appended++, j, 1));
		for(int i=0;i<3;i++)
		{
			delta(row,i)=vesq[st]->math_data()->v[i];
		}
		row++;
	}

	slaplacian.setFromTriplets(tripletList.begin(), tripletList.end());

	// Solving:
	//SparseQR<SparseMatrix<float>, COLAMDOrdering<int> > qr(slaplacian);
	//SpMat R = qr.matrixR();
	//SpMat L = R.transpose()*R;
	//MatrixXf b = slaplacian.transpose()*delta;
	///*qr.compute(slaplacian);*/
	///*qr.analyzePattern(slaplacian);
	//qr.factorize(slaplacian);*/
	//MatrixXf result = L.solve(b);

	SpMat A = slaplacian.transpose()*slaplacian;
	SparseLU<SparseMatrix<float>, COLAMDOrdering<int> > lu;
	lu.analyzePattern(A);
	lu.factorize(A);
	MatrixXf deltaa = slaplacian.transpose()*delta;
	MatrixXf result = lu.solve(deltaa);
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->newdata.v[0] = (result.col(0))((*iv)->name-1);
		(*iv)->newdata.v[1] = (result.col(1))((*iv)->name-1);
		(*iv)->newdata.v[2] = (result.col(2))((*iv)->name-1);

		(*iv)->color[0] = color_emum[0][0];
		(*iv)->color[1] = color_emum[0][1];
		(*iv)->color[2] = color_emum[0][2];
	}
	for(vector<Vertex*>::size_type st=0;st<anchor;st++)
	{
		vesq[st]->color[0] = color_emum[1][0];
		vesq[st]->color[1] = color_emum[1][1];
		vesq[st]->color[2] = color_emum[1][2];
	}
	
}
void Mesh::block_least_square(int bit,int anchor)
{
	MathVector c;
	int count=0;
	int row;
	typedef Eigen::Triplet<float> T;
	std::vector<T> tripletList;
	vector<Vertex*> seedsq;
	int color =0;
	int color_emum[8][3]={{255,97,0},{127,255,0},{218,112,214},{128,42,42},{8,46,84},{255,99,71},{65,105,225},{48,128,20}};//橙色 黄绿色 淡紫色 米色 靛青 蕃茄红 品蓝 暗绿色
	list<Vertex*> *part_list;
	for(vector<list<Vertex*>>::size_type liv = 0; liv != superlist.size();liv++)//每一块设置ls坐标
	{
		part_list = &superlist[liv];
		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			for(list<Vertex*>::iterator ivv=((*it)->segmented_neighborvertex).begin();ivv!=((*it)->segmented_neighborvertex).end();ivv++)
			{
				c.add((*ivv)->math_data());
				count++;
			}
			c.scale(1.0/count);
			count=0;
			c.sub((*it)->math_data());
			c.scale(-1.0);
			(*it)->lapcoor.set(c.v[0],c.v[1],c.v[2]);
			c.set_zero();
		}
	}
	//整体量化
	float the_max;
	float the_min;
	float chazhi;
	float max_error;
	for(int i=0; i<3; i++)
	{
		the_max=-100;
		the_min=100;
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			the_max = max(the_max,(*iv)->math_lapcoor()->v[i]);
			the_min = min(the_min,(*iv)->math_lapcoor()->v[i]);
		}
		chazhi = the_max - the_min;
		max_error = chazhi*pow(2.0,-bit);
		cout<<"max_error: "<<max_error<<endl;
		for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
		{
			(*iv)->math_quantized_lapcoor()->v[i] = floor(((*iv)->math_lapcoor()->v[i]-the_min)/max_error +0.5)*max_error +the_min;
		}
	}

	for(vector<list<Vertex*>>::size_type liv = 0; liv != superlist.size();liv++)//每一块
	{
		printf("delta...\n");
		part_list = &superlist[liv];
		MatrixXf delta = MatrixXf::Zero(part_list->size()+anchor,3);
		SpMat slaplacian(part_list->size()+anchor,part_list->size());
		int i,j;
		tripletList.clear();
		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{
			i = (*it)->blockid-1;
			tripletList.push_back(T(i, i, (*it)->segmented_neighborvertex.size()));
			for(int ii=0;ii<3;ii++)
			{
				delta((*it)->blockid-1,ii) = (*it)->math_quantized_lapcoor()->v[ii]*(*it)->segmented_neighborvertex.size();
			}
			for(list<Vertex*>::iterator ivv=((*it)->segmented_neighborvertex).begin();ivv!=((*it)->segmented_neighborvertex).end();ivv++)
			{
				j = (*ivv)->blockid-1;
				tripletList.push_back(T(i, j, -1.0));
			}
			if((*it)->dye==0)
				seedsq.push_back((*it));
		}
		slaplacian.setFromTriplets(tripletList.begin(), tripletList.end());
		
		int appended = part_list->size();
		for(vector<Vertex*>::size_type st=0;st<anchor;st++)
		{
			j = seedsq[st]->blockid-1;
			tripletList.push_back(T(appended, j, 1));
			for(int i=0;i<3;i++)
			{
				delta(appended,i)=seedsq[st]->math_data()->v[i];
			}
			appended++;
		}
		seedsq.clear();
		// Solving:
		SparseQR<SparseMatrix<float>, COLAMDOrdering<int> > qr;
		/*qr.compute(slaplacian);*/
		qr.analyzePattern(slaplacian);
		qr.factorize(slaplacian);
		MatrixXf result = qr.solve(delta);
		//保存求解的坐标值
		for(list<Vertex*>::iterator it = part_list->begin(); it != part_list->end(); it++)
		{

			(*it)->newdata.v[0] = (result.col(0))((*it)->blockid-1);
			(*it)->newdata.v[1] = (result.col(1))((*it)->blockid-1);
			(*it)->newdata.v[2] = (result.col(2))((*it)->blockid-1);
			(*it)->color[0] = color_emum[color%8][0];
			(*it)->color[1] = color_emum[color%8][1];
			(*it)->color[2] = color_emum[color%8][2];
		}
		color++;
	}
}

void Mesh::compute_vertex_curvature()
{
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->computer_GaussCurvature();
	}
	/*fstream f("documents\\curvature.txt",ios::out);
	int count_01=0,count_12=0,count_2=0;
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		f<<(*iv)->curvature<<endl;
		if(abs((*iv)->curvature)<1.0)
			count_01++;
		else if(abs((*iv)->curvature)<2.0)
			count_12++;
		else
			count_2++;
	}
	f.close();
	printf("0-1: %d,1-2:%d,>2:%d\n",count_01,count_12,count_2);*/
	
}

void Mesh::compute_vertex_square_devaition_angle()
{
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		(*iv)->compute_square_devaition_angle();
	}

	fstream f("documents\\square_devaition_angle.txt",ios::out);
	for(list<Vertex*>::iterator iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		f<<(*iv)->square_devaition_angle<<endl;
	}
	f.close();
}

bool Sqlarge(const Vertex* v1, const Vertex* v2)
{
	return abs(v1->to_anthor_length) > abs(v2->to_anthor_length);
}

string convertToString(int x)
{
	ostringstream o;
	if (o << x)
		return o.str();
	return "conversion error";
}