//
//    File: ply_mesh.cc
//
//    (C) 2000 Helmut Cantzler / 2004 Toby Breckon
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "ply_mesh.h"

int PLY_Mesh::read(FILE *f, int (*update_progress)(int pos),
				   void (*set_total)(int size))
{
	list<Triangle*> *shape_triangles;
	list<Vertex*> *shape_vertices;
	vector<Vertex*> vertex_index;
	int p1, p2, p3, i, v_nr, p_nr;
	char nr; 
	float x, y, z;
	Triangle *tri;
	Vertex *v;

	char buffer[101];

	fseek(f, 0, SEEK_END);
	(*set_total)(ftell(f));
	fseek(f, 0, SEEK_SET);

	// READ IN HEADER

	while (fgetc(f) != '\n') // Reads all of the 1st line
		;

	// read the remainder of the header
	// - interested in 3 parts only
	// - format
	// - number of vertices
	// - number of polygons

	fscanf(f,"%100s ", buffer);
	while (strcasecmp(buffer, "end_header") != 0){

		if (strcasecmp(buffer, "format") == 0){

			fscanf(f,"%100s ", buffer);
			if (strcasecmp(buffer, "ascii") != 0){
				FILE_ERROR(f, 
					"PLY file format error: PLY ASCII support only.");
				return 3;
			}

		} else if (strcasecmp(buffer, "element") == 0){
			fscanf(f,"%100s ", buffer);
			if (strcasecmp(buffer, "vertex") == 0){
				fscanf(f,"%100s", buffer);
				v_nr = atoi(buffer);
			} else if (strcasecmp(buffer, "face") == 0){
				fscanf(f,"%100s", buffer);
				p_nr = atoi(buffer);
			}
		}
		fscanf(f,"%100s ", buffer);
	}

	shape_triangles = new list<Triangle*>;
	shape_vertices = new list<Vertex*>;

	// READ IN VERTICES

	for (i=0; i < v_nr; i++) // Reads the vertices
	{
		if (fscanf(f,"%f %f %f",&x,&y,&z) != 3)
		{
			FILE_ERROR(f, "PLY file format error: vertex list");
			return 3;
		}
		while (fgetc(f) != '\n'); // Read till end of the line
		// to skip texture/colour values

		v = new Vertex(x,y,z);
		add_vertex(v);

		// save in a vector for the triangles and shape
		vertex_index.push_back(v);

		if ((*update_progress)(ftell(f)))
			return 90;
	}

	// READ IN POLYGONS

	while ((0 < p_nr) && (!(feof(f)))) 
	{
		fscanf(f,"%c", &nr);

		switch (nr)
		{
		case '2':
			if (fscanf(f,"%d %d\n", &p1, &p2) != 2)
			{
				FILE_ERROR(f, "PLY file format error: polygon 2");
				return 6;
			}
			add_edge(new Edge(vertex_index[p1], vertex_index[p2]));
			p_nr--;
			break;

		case '3':
			if (fscanf(f,"%d %d %d\n", &p1, &p2, &p3) != 3)
			{
				FILE_ERROR(f, "PLY file format error: polygon 3");
				return 7;
			}
			if ((p1 >= ver_nr) || (p2 >= ver_nr) || (p3 >= ver_nr)){
				FILE_ERROR(f, "PLY file format error: vertex index out of range");
				return 8;
			} else { 
				tri = new Triangle(vertex_index[p1], vertex_index[p2],
					vertex_index[p3]);
				add_triangle(tri);
				p_nr--;
			}
			// save in a list for the shape
			shape_triangles->push_back(tri);
			break;

		case ' ':
		case '\t':
		case '\n':
			// skip leading whitespace characters
			break;

		default:
			// skip any lines that that we are not interested in
			// (i.e. don't begin with the cases above)
			// e.g. N vertex polygons of form "N v1 v2...vN" on the line
			do 
			{
				nr=fgetc(f);
			}
			while ((!(feof(f))) && nr != '\n');
			break;
		}

		if ((*update_progress)(ftell(f)))
			return 90;
	}

	// copy the vertices of this shape over
	shape_vertices->insert(shape_vertices->begin(),
		vertex_index.begin(), vertex_index.end());
	shapes->push_back(new Shape(shape_triangles, shape_vertices));

	return 0;
}

void PLY_Mesh::write(FILE *f, const char *comment)
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
	fprintf(f, "element face %i\n", tri_nr);
	fprintf(f, "property list uchar int vertex_indices\n");
	fprintf(f, "end_header\n");

	n=0;    // vertices
	for (iv=vertices->begin(); iv != vertices->end(); iv++)
	{
		/*calc_original_coordinates(*iv, &v);
		fprintf(f,"%f %f %f\n", v.x(), v.y(), v.z());*/
		fprintf(f,"%f %f %f\n", (*iv)->x(), (*iv)->y(), (*iv)->z());
		(*iv)->number=++n;
	}

	// triangles
	for (it=triangles->begin(); it != triangles->end(); it++)
		fprintf(f,"3 %d %d %d\n", ((*it)->vertices[0]->number - 1),
		((*it)->vertices[1]->number - 1), ((*it)->vertices[2]->number) - 1);
}
