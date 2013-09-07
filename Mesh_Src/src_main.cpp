#include "ply_mesh.h"
#include "glmesh.h"

int dummy1(int pos)
{
	return 0;
}

void dummy2(int size)
{
}

int main(int argc, char **argv)
{
	GLMesh_Settings settings;

	FILE *f;

	//settings.mesh = new Mesh();
	settings.mesh_displayMode = GPOINTS;
	// settings.mesh_displayMode = SALIENCY_COLOR; 

	settings.features_displayMode = NOTHING;
	settings.xShift = settings.yShift = 0.0;
	settings.zShift = 0.0;
	settings.clipping = 0.0;
	settings.light_brightness = 1.0;
	settings.red = settings.green = settings.blue = 0.0;
	settings.f_normals = settings.f_different_colors_for_shapes = 0;
	settings.f_texture_filter = settings.f_back_faces = 0;
	// settings.f_lighting = 0;  
	settings.f_features = 0;
	settings.f_aspect_ratio = 1;

	for (int i=0; i < 4; i++)
		for (int j=0; j < 4; j++)
			settings.tb_transform[i][j]= i == j ? 1.0 : 0.0;

	if ((f = fopen("model\\wolf2534.ply","r")) == NULL)
	{
		fprintf(stderr, "Can't open %s for reading\n", argv[1]);
		exit(0);
	}
	settings.mesh =  new PLY_Mesh();
	settings.mesh->read(f, (int (*)(int)) dummy1, (void (*)(int)) dummy2);
	fclose(f);
	settings.mesh->create_edges();
	printf("\nMesh: %d vertices, %d edges, %d triangles\n",
	settings.mesh->number_of_vertices(),
	settings.mesh->number_of_edges(),
	settings.mesh->number_of_triangles());

	// Map vertices and triangle centroids to mesh centroid
	printf("Map mesh to centre ...\n");
	settings.mesh->move_to_centre();
	printf("Scale mesh ...\n");
	settings.mesh->scale_into_normal_sphere();

	if((f=fopen("documents\\wolf2534.ply","w"))==NULL)
	{
		fprintf(stderr, "Can't open %s for reading\n", argv[1]);
		exit(0);	
	}
	settings.mesh->write(f, "VCGLIB generated");
	fclose(f);
	openglInit(settings);
	printf("Start displaying ...\n");
	openglStart();
}
