//
//    File: glmesh.h
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _OPENGL_FUNCTIONS_H
#define _OPENGL_FUNCTIONS_H
#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include "mesh.h"

// Types

enum display_type {GPOINTS=0, EDGES, TEXTURE, SOLID, WIRE,
		   FRONTLINES, FEATURES, NOTHING};
enum action_type {NONE, TRANSLATE, ROTATE, ZOOM};

typedef struct
{
  Mesh *mesh, *features;
 
  // Viewpoint data (Trackball, Translation, Zoom, Clipping)
  GLfloat tb_transform[4][4];
  GLfloat xShift, yShift, zShift;
  GLfloat clipping;
 
  // Viewmode
  enum display_type mesh_displayMode, features_displayMode;
 
  // Light intensity
  GLfloat light_brightness;
 
  // Background colour
  GLfloat red, green, blue;

  // Flags
  int f_normals, f_different_colors_for_shapes, f_features;
  int f_texture_filter, f_back_faces, f_lighting, f_aspect_ratio;
} GLMesh_Settings;

// Colours
enum colors {BLACK=0, BLUE, GREEN, RED, YELLOW, PURPLE, CYAN, WHITE};

const GLfloat ambientLightSource[] = {0.0, 0.0, 0.0, 1.0};

extern GLMesh_Settings settings;

extern float surface_normal_length;
extern int number_of_shapes;
extern GLuint *textureList;
extern GLint windowWidth;
extern GLint windowHeight;
extern GLint windowX;
extern GLint windowY;
extern int lastMouseX, lastMouseY;
extern enum action_type actionMode;
extern GLfloat tb_lastposition[3];
extern GLfloat tb_axis[3];
extern int translation_zoom;

void draw(void);
void reshape(int width, int height);
void setTitle(void);
void key(unsigned char k, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void main_menu(int value);
void display_menu(int value);
void openglInit(GLMesh_Settings s);
void openglStart(void);

void checkForTraps(const char *string, int value);
void setMaterialColor(GLenum face, GLenum pname, int color);
void setMaterialColorGrey(GLenum face, GLenum pname, int b);
void displayNormals(void);
void displayFeatures(void);
void displayPoints(list<Vertex*> *vertices);
void displayEdges(list<Edge*> *edges);
void displayMesh(void);
void displayMeshWithStencil(void);
void toggleTextureFilter(void);
void tbPointToVector(int x, int y, float v[3]);
void initLighting(void);
void initTexture(void);
void setShapeTexture(int j);

int width(void);
int height(void);

#endif
