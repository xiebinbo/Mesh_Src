#include "ply_mesh.h"
#include "glmesh.h"
#include<fstream>

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

	if ((f = fopen("model\\happy16268.ply","r")) == NULL)	//Armadilloascii18245  horse4243 Laurana6301 D20000risu_noise fandisk skull blade10793 male10042 raptor7445 cow2903 bun_zipper17446 bun_zipper8747 blade5350
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

	settings.mesh->ini_ner_ver();
	//settings.mesh->compute_vertex_curvature();
	//settings.mesh->compute_vertex_square_devaition_angle();
	//settings.mesh->lookfor_seeds(1);
	//settings.mesh->segment_graph();		//模型分割
	//settings.mesh->ini_segmented_ner_ver();	//计算分割后模型的topo-1
	//settings.mesh->ini_segmented_nei2_ver();	//计算分割后模型的topo-2
	//settings.mesh->greedy_packet();			//着色 分包

	//settings.mesh->test_anchor();
	//if((f=fopen("documents\\anchor.txt","w"))==NULL)
	//{
	//	exit(0);
	//}
	//for(int i=5;i>2;i--)
	//{
	//settings.mesh->statistics_data(f,i);
	//}
	//fclose(f);
	//....................................................
	//fstream fs("documents\\mlength.txt",ios::out);
	//fs.seekp(ios::beg);
	//string ss = "documents\\blade5350bBFS\\blade5350";
	//string s1(ss);
	//for(int bit=3;bit<=8;bit++)
	//{
	//	s1.append("b");
	//	s1.append(convertToString(bit));
	//	s1.append("BFS");
	//	settings.mesh->test_anchor(bit,fs,s1);
	//	s1 = ss;
	//}
	//fs.close();
	//....................................................
	settings.mesh->test_anchor3();
	//....................................................................................
	//argv[1] = "documents\\horse4243b1.ply";
	//argv[2] = "documents\\horse4243b2.ply";
	//argv[3] = "documents\\horse4243b3.ply";
	//argv[4] = "documents\\horse4243b4.ply";
	//argv[5] = "documents\\horse4243b5.ply";
	//argv[6] = "documents\\horse4243b6.ply";
	//argv[7] = "documents\\horse4243b7.ply";
	//argv[8] = "documents\\horse4243b8.ply";
	//argv[9] = "documents\\horse4243b9.ply";
	//argv[10] = "documents\\horse4243b10.ply";
	//argv[11] = "documents\\horse4243b11.ply";
	//argv[12] = "documents\\horse4243b12.ply";
	//argv[13] = "documents\\horse4243b13.ply";
	//argv[14] = "documents\\horse4243b14.ply";
	//argv[15] = "documents\\horse4243b15.ply";
	//int anchor[10] = {5,10,20,30,40,50,60,70,80,90};//各个锚点//5,10,15,20,25,30,35,40,45,50,55,60  10,20,30,40,50,60,70,80,90
	//settings.mesh->quantizing_laplacian_coordinates(5);//量化
	//settings.mesh->initialize_tripletList_and_deltamesh();
	//settings.mesh->the_min = 100;
	//for(int i=0;i<10;i++)
	//{
	//	cout<<"anchor: "<<anchor[i]<<endl;
	//	settings.mesh->vesq.clear();
	//	settings.mesh->vesq.push_back(settings.mesh->first_anchor);//选好第一个锚点
	//	settings.mesh->iterator_compute_number(anchor[i]);
	//	settings.mesh->ccccc();
	//	if((f=fopen(argv[i+1],"w"))==NULL)
	//	{
	//		exit(0);	
	//	}
	//	settings.mesh->write(f, "VCGLIB generated");
	//	fclose(f);
	//}

	//.....................................................................................

	//settings.mesh->solve_sparse_linear_equations();
	//settings.mesh->visual_quality_measure(0.3);
	//settings.mesh->solving_least_squares(settings.mesh->vesq.size());
	//settings.mesh->block_least_square(8,16);
	//...................................................................
	/*if((f=fopen("documents\\horse4243BFS10.ply","w"))==NULL)
	{
	fprintf(stderr, "Can't open %s for reading\n", argv[1]);
	exit(0);	
	}
	settings.mesh->write(f, "VCGLIB generated");
	fclose(f);*/
	//..................................................................
	//openglInit(settings);
	printf("Start displaying ...\n");
	//openglStart();
}
