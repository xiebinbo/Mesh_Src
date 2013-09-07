//
//    File: glmesh.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "glmesh.h"

// Texts
#define MAIN_MENU_SIZE 12
#define DISPLAY_MENU_SIZE 6
#define ASCII_ESCAPE 27

struct {
	char* text;
	char key;
} main_menu_text[MAIN_MENU_SIZE] = { {"Toggled translation/zoom (t)", 't'},
{"Toggled texture filter (T)", 'T'},
{"Toggled features (f)", 'f'},
{"Toggled normals (n)", 'n'},
{"Toggled lighting (l)", 'l'},
{"Toggled shape colors (s)", 's'},
{"Lighting up (u)", 'u'},
{"Lighting down (d)", 'd'},
{"Clipping up (+)", '+'},
{"Clipping down (-)", '-'},
{"Full screen (F)", 'F'},
{"Quit (esc)", ASCII_ESCAPE} };

struct {
	char* text;
	char key;
} display_menu_text[DISPLAY_MENU_SIZE] = { {"Texture (1)", '1'},
{"Solid (2)", '2'},
{"Frontlines (3)", '3'},
{"Wireframe (4)", '4'},
{"GPOINTS (5)", '5'},
{"Features (6)", '6'} };

char title[100];

GLMesh_Settings settings;

float surface_normal_length;
int number_of_shapes;
GLuint *textureList;
GLint windowWidth = 1000;//500
GLint windowHeight = 1000;//500
GLint windowX = 0;//100
GLint windowY = 0;
int lastMouseX, lastMouseY;
enum action_type actionMode = NONE;
GLfloat tb_lastposition[3];
GLfloat tb_axis[3];
int translation_zoom;

////////////////// GLUT handlers ////////////////// 

void draw(void)
{
	// Clear the current display
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// save matrix
	glPushMatrix();

	// Change the modelview transformation
	glMultMatrixf((GLfloat*) settings.tb_transform);

	// Change the projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.1 - settings.xShift - settings.zShift,
		2.1 - settings.xShift + settings.zShift,
		-2.1 - settings.yShift - settings.zShift,
		2.1 - settings.yShift + settings.zShift,
		-2.1 - settings.zShift - settings.clipping, 2.1);
	glMatrixMode(GL_MODELVIEW);

	// display main object (typically a mesh)
	switch (settings.mesh_displayMode)
	{
	case GPOINTS:
		setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, CYAN);
		displayPoints(settings.mesh->get_vertices());
		break;

	case EDGES:
		setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, CYAN);
		displayEdges(settings.mesh->get_edges());
		break;

	case SOLID:
	case TEXTURE:
		// Set polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		displayMesh();
		break;

	case FRONTLINES:
		displayMeshWithStencil();
		break;

	case WIRE:
		// Set polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		displayMesh();
		break;

	case FEATURES:
		displayFeatures();
		break;

		//    case FRONTLINES2:
		// draw the solid
		//      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//      display_mesh();
		// draw the the wireframe on top of the solid
		//      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//      glDepthFunc(GL_LEQUAL);
		//      display_mesh();
		//      glDepthFunc(GL_LESS);
		//      break;
	}

	checkForTraps("Draw Polygons", 1);

	// Draw features like GPOINTS and edges additionally
	if (settings.f_features && settings.mesh_displayMode != FEATURES)
		displayFeatures();

	checkForTraps("Draw Features", 1);

	// Draw the surface normals
	if (settings.f_normals)
		displayNormals();

	checkForTraps("Draw Surface Normals",1);

	glPopMatrix();
	glFlush();
	glutSwapBuffers(); // Needed when double buffering
}

void mydraw(void)
{
	// Clear the current display
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// save matrix
	glPushMatrix();

	// Change the modelview transformation
	glMultMatrixf((GLfloat*) settings.tb_transform);

	// Change the projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.1 - settings.xShift - settings.zShift,
		2.1 - settings.xShift + settings.zShift,
		-2.1 - settings.yShift - settings.zShift,
		2.1 - settings.yShift + settings.zShift,
		-2.1 - settings.zShift - settings.clipping, 2.1);
	glMatrixMode(GL_MODELVIEW);

	glViewport(0,0,500,500);
	setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, CYAN);
	displayPoints(settings.mesh->get_vertices());
	
	glViewport(500,0,500,500);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	displayMesh();

	glViewport(0,500,500,500);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	displayMesh();
	
	glViewport(500,500,500,500);
	setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, YELLOW);
	displayEdges(settings.mesh->get_edges());

	checkForTraps("Draw Polygons", 1);

	// Draw features like GPOINTS and edges additionally
	if (settings.f_features && settings.mesh_displayMode != FEATURES)
		displayFeatures();

	checkForTraps("Draw Features", 1);

	// Draw the surface normals
	if (settings.f_normals)
		displayNormals();

	checkForTraps("Draw Surface Normals",1);

	glPopMatrix();
	glFlush();
	glutSwapBuffers(); // Needed when double buffering
}
void reshape(int width, int height)
{
	int max = width > height ? width : height;

	if (settings.f_aspect_ratio)
		glViewport((width-max)/2, (height-max)/2, max, max);
	else
		glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;
}

void setTitle(void)
{
	strcpy(title, "Mesh Viewer -- ");

	if (translation_zoom)
		strcat(title, "Translation");
	else
		strcat(title, "Zoom");
	strcat(title, " -- ");

	switch (settings.mesh_displayMode)
	{
	case TEXTURE:
		strcat(title, "Texture");
		break;
	case SOLID:
		strcat(title, "Solid");
		break;
	case FRONTLINES:
		strcat(title, "Frontlines");
		break;
	case WIRE:
		strcat(title, "Wireframe");
		break;
	case GPOINTS:
		strcat(title, "GPOINTS");
		break;
	case FEATURES:
		strcat(title, "Features");
		break;
	}

	glutSetWindowTitle(title);
}

void toggleTextureFilter(void)
{
	GLint filter;
	int id;

	settings.f_texture_filter = !settings.f_texture_filter;
	filter = settings.f_texture_filter ? GL_LINEAR : GL_NEAREST;

	for (int i=0; i < number_of_shapes; i++)
	{
		id=settings.mesh->get_shape(i)->get_texture_id();

		if (id != -1)
		{
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		}
	}
}

void key(unsigned char k, int x, int y)
{

	switch (k) 
	{
	case ASCII_ESCAPE: // Quit (Esc)

		glDeleteTextures(number_of_shapes, textureList);
		delete textureList;
		delete settings.mesh;

		exit(0);

	case 'T':
		toggleTextureFilter();
		break;

	case 'f':
		settings.f_features=!settings.f_features;
		break;

	case '1':
		settings.mesh_displayMode=TEXTURE;
		setTitle();
		break;
	case '2':
		settings.mesh_displayMode=SOLID;
		setTitle();
		break;
	case '3':
		settings.mesh_displayMode=FRONTLINES;
		setTitle();
		break;
	case '4':
		settings.mesh_displayMode=WIRE;
		setTitle();
		break;
	case '5':
		settings.mesh_displayMode=GPOINTS;
		setTitle();
		break;
	case '6':
		settings.mesh_displayMode=FEATURES;
		setTitle();
		break;

	case 'b':
		settings.f_back_faces=!settings.f_back_faces;
		if (settings.f_back_faces)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		break;

	case 'l': // Toggled lighting (l)
		settings.f_lighting=!settings.f_lighting;
		if (settings.f_lighting) 
			glEnable(GL_LIGHTING);
		else 
			glDisable(GL_LIGHTING);
		break;

	case 's': // Toggled shape colors (s)
		settings.f_different_colors_for_shapes=
			!settings.f_different_colors_for_shapes;
		break;

	case 't': // Toggled translation/zoom (t)
		translation_zoom=!translation_zoom;
		setTitle();
		break;

	case 'F': // Full screen (f)
		glutFullScreen();
		break;

	case 'n': // Toggled normals (n)
		settings.f_normals=!settings.f_normals;
		break;

	case 'N': // Negate surface normals (N)
		settings.mesh->negate_surface_normals();
		break;

	case '=': case '+': // Clipping up (+)
		settings.clipping+=0.2;
		break;

	case '-': // Clipping down (-)
		settings.clipping-=0.2;
		break;

	case 'u': // Light up (u)
		{
			settings.light_brightness+=settings.light_brightness*0.1;
			GLfloat source[] = {settings.light_brightness,
				settings.light_brightness,
				settings.light_brightness,
				settings.light_brightness};

			glLightfv(GL_LIGHT0, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT3, GL_DIFFUSE, source);
		}
		break;

	case 'd': // Light down (d)
		{
			settings.light_brightness-=settings.light_brightness/1.1*0.1;
			GLfloat source[] = {settings.light_brightness,
				settings.light_brightness,
				settings.light_brightness,
				settings.light_brightness};

			glLightfv(GL_LIGHT0, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, source);
			glLightfv(GL_LIGHT3, GL_DIFFUSE, source);
		}
		break;
	}

	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
	switch(state)
	{
	case GLUT_DOWN:

		lastMouseX = x;
		lastMouseY = y;

		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			actionMode = ROTATE;
			tbPointToVector(x, y, tb_lastposition);
			break;
			//      case GLUT_RIGHT_BUTTON:
		case GLUT_MIDDLE_BUTTON:
			if (translation_zoom)
				actionMode = TRANSLATE;
			else
				actionMode = ZOOM;
			break;
		}
		break;

	case GLUT_UP:
		actionMode = NONE;
	}
}

void mouseDrag(int x, int y)
{
	switch (actionMode)
	{
	case ZOOM:
		settings.zShift += (y-lastMouseY)/(double)windowHeight*3;
		lastMouseX = x;
		lastMouseY = y;
		glutPostRedisplay();
		break;

	case TRANSLATE:
		settings.xShift += (x-lastMouseX)/(double)windowWidth*3;
		settings.yShift -= (y-lastMouseY)/(double)windowHeight*3;
		lastMouseX = x;
		lastMouseY = y;
		glutPostRedisplay();
		break;

	case ROTATE:
		float current_position[3], angle, dx, dy, dz;

		tbPointToVector(x, y, current_position);

		dx = current_position[0] - tb_lastposition[0];
		dy = current_position[1] - tb_lastposition[1];
		dz = current_position[2] - tb_lastposition[2];
		angle = 90.0 * sqrt(dx * dx + dy * dy + dz * dz) * 2;

		tb_axis[0] = tb_lastposition[1] * current_position[2] -
			tb_lastposition[2] * current_position[1];
		tb_axis[1] = tb_lastposition[2] * current_position[0] -
			tb_lastposition[0] * current_position[2];
		tb_axis[2] = tb_lastposition[0] * current_position[1] -
			tb_lastposition[1] * current_position[0];

		tb_lastposition[0] = current_position[0];
		tb_lastposition[1] = current_position[1];
		tb_lastposition[2] = current_position[2];

		glPushMatrix();
		glLoadIdentity();
		glRotatef(angle, tb_axis[0], tb_axis[1], tb_axis[2]);
		glMultMatrixf((GLfloat*) settings.tb_transform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)settings.tb_transform);
		glPopMatrix();

		glutPostRedisplay();
		break;
	}
}

void mainMenu(int value)
{
	key(main_menu_text[value].key,0,0);
}

void displayMenu(int value)
{
	key(display_menu_text[value].key,0,0);
}

////////////////// Intialisation ////////////////// 

void openglInit(GLMesh_Settings s)
{
	settings=s;

	settings.mesh_displayMode=NOTHING;
	if (settings.mesh != NULL)
	{
		if (settings.mesh->number_of_triangles() > 0)
			settings.mesh_displayMode=GPOINTS;//settings.mesh_displayMode=SOLID;
		else if (settings.mesh->number_of_edges() > 0)
			settings.mesh_displayMode=EDGES;
		else if (settings.mesh->number_of_vertices() > 0)
			settings.mesh_displayMode=GPOINTS;

		number_of_shapes = settings.mesh->number_of_shapes();
		surface_normal_length = 0.02 + settings.mesh->average_triangle_size()/10;
	}
	else
		number_of_shapes = 0;

	/* settings.features_displayMode=NOTHING;
	if (settings.features != NULL)
	if (settings.features->number_of_edges() > 0)
	settings.features_displayMode=EDGES;
	else if (settings.features->number_of_vertices() > 0)
	settings.features_displayMode=GPOINTS;

	actionMode=NONE;*/

	// Initialise the display window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(windowX, windowY);
	glutCreateWindow("");
	setTitle();

	// Set background colour and clear
	glClearColor(0.0, 0.0, 0.0, 0.0);  // Black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Initialise light source
	initLighting();

	// Other initialisations
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glLineWidth(1.5);
	glPointSize(1.5);
	glEnable(GL_POINT_SMOOTH);

	if (settings.f_back_faces)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Enable Smooth Shading
	//glShadeModel(GL_SMOOTH);
	// Really Nice Perspective Calculations
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Initialise the modelview transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Texture stuff
	printf("Initialise Texture ...\n");
	initTexture();

	// Make the menu
	glutCreateMenu(mainMenu);
	glutCreateMenu(displayMenu);
	for (int i=0; i < DISPLAY_MENU_SIZE; i++)
		glutAddMenuEntry(display_menu_text[i].text, i);
	glutSetMenu(1);
	glutAddSubMenu("Display", 2);
	for (int i=0; i < MAIN_MENU_SIZE; i++)
		glutAddMenuEntry(main_menu_text[i].text, i);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void openglStart(void)
{
	// Register callback handlers
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);
	glutIdleFunc(0);

	// Main (infinite) loop 
	glutMainLoop();
}

int width(void)
{
	return windowWidth;
}

int height(void)
{
	return windowHeight;
}
