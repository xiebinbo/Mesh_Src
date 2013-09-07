//
//    File: glmesh_common.cc
//
//    (C) 2000 Helmut Cantzler
//
//    Licensed under the terms of the Lesser General Public License.
//

#include "glmesh.h"

void checkForTraps(const char *string, int value)
{
  GLenum error_code = glGetError();
  if( error_code!= GL_NO_ERROR)
    {

      printf("\n\n\n");
      printf("During processing %s (%d) an internal error occurred!\n",
	     string, value);
      printf("OpenGL error: %d ", error_code);
      printf("(%s)\n", gluErrorString(error_code));
      exit(0);
    }
}

void setMaterialColor(GLenum face, GLenum pname, int color)
{
  static float colors[8][4] = {{0.0, 0.0, 0.0, 0.0},  // black
			       {0.0, 0.0, 1.0, 1.0},  // blue
			       {0.0, 1.0, 0.0, 1.0},  // green
			       {1.0, 0.0, 0.0, 1.0},  // red
			       {1.0, 1.0, 0.0, 1.0},  // yellow
			       {1.0, 0.0, 1.0, 1.0},  // purple
			       {0.0, 1.0, 1.0, 1.0},  // cyan
			       {1.0, 1.0, 1.0, 1.0}}; // white
  float b;
  int c, j;

  color--;
  // b = 0.0 until 0.9
  b = color / 7 < 10 ? color / 7 * 0.1 : 0.9;
  // c = 0 until 7
  c = color != -1 ? color % 7 + 1 : 0;
 
  for (j=0; j < 3; j++)
    colors[c][j]= colors[c][j] ? 1.0 - b : 0.0;

  glMaterialfv(face, pname, colors[c]);
}

void setMaterialColorGrey(GLenum face, GLenum pname, int b)
{
  static float grey[4] = {1.0, 1.0, 1.0, 1.0};
  int j;

  for (j=0; j < 3; j++)
    grey[j] = 1.0 - 0.15 * b;

  glMaterialfv(face, pname, grey);
}

void displayNormals(void)
{
  list<Triangle*>::iterator it;
  list<Triangle*> *triangles;

  setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BLUE);
  glBegin(GL_LINES);
   for (int i=0; i < number_of_shapes; i++)
     {
       triangles=settings.mesh->get_shape(i)->triangles;
	   
       if (triangles != NULL)
	 for (it=triangles->begin(); it != triangles->end(); it++)
	   {
	     const float *dnorm = (*it)->float_normal();
	     const float *dcent = (*it)->centroid();
	       
	     glVertex3fv((GLfloat*) dcent);
	     glVertex3f( dcent[0] + surface_normal_length*dnorm[0],
			 dcent[1] + surface_normal_length*dnorm[1],
			 dcent[2] + surface_normal_length*dnorm[2]);
	   }
     }
  glEnd();
}

void displayPoints(list<Vertex*> *vertices)
{
  list<Vertex*>::iterator iv;

   for (iv=vertices->begin(); iv != vertices->end(); iv++)
     {
       glLoadName((*iv)->name);
       glBegin(GL_POINTS);
        if ( (*iv)->float_normal() != NULL )
	  glNormal3fv( (GLfloat*) (*iv)->float_normal() );
	glVertex3fv( (GLfloat*) (*iv)->float_data() );
       glEnd();
     }
}

void displayEdges(list<Edge*> *edges)
{
  list<Edge*>::iterator ie;

   for (ie=edges->begin(); ie != edges->end(); ie++)
    {
      glLoadName((*ie)->name);
      glBegin(GL_LINES);
       glVertex3fv((GLfloat*) (*ie)->vertices[0]->float_data() );
       glVertex3fv((GLfloat*) (*ie)->vertices[1]->float_data() );
      glEnd();
    }
}

void displayFeatures()
{
  setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, RED);
  //setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BLACK);

  //glDisable(GL_DEPTH_TEST);

  switch (settings.features_displayMode)
    {
    case GPOINTS:
      displayPoints(settings.features->get_vertices());
      break;

    case EDGES:
      displayEdges(settings.features->get_edges());
      break;
    }

  //glEnable(GL_DEPTH_TEST);
}

void displayShapePoints(void)
{
  list<Vertex*> *vertices;
  Shape* shape;
  int nr;

  for (int i=0; i < number_of_shapes; i++)
    {
      shape=settings.mesh->get_shape(i);
      vertices=shape->vertices;
	
      if (vertices != NULL)
	{
	  if (!settings.f_different_colors_for_shapes || 
	      i+1 == number_of_shapes)
	    nr = CYAN; // BLUE;
	  else
	    nr = i+1 == CYAN ? i+2 : i+1;

	  // Set color
	  setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, nr);
	  
	  displayPoints(vertices);
	}
    }
}

void displayMesh(void)
{
  list<Triangle*>::iterator it;
  list<Triangle*> *triangles;
  Shape* shape;
  int nr;

  for (int i=0; i < number_of_shapes; i++)
    {
      shape=settings.mesh->get_shape(i);
      triangles=shape->triangles;
	
      if (triangles != NULL)
	if (settings.mesh_displayMode == TEXTURE && shape->get_texture_id() != -1)
	  {
	    setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WHITE);
	    
	    glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, shape->get_texture_id());
	    
	    for (it=triangles->begin(); it != triangles->end(); it++)
	      {
		glLoadName((*it)->name);
		glBegin(GL_TRIANGLES);

		 glNormal3fv( (GLfloat*) (*it)->float_normal() );

		 glTexCoord2f( (GLfloat) (*it)->get_text_s(0),
			       (GLfloat) (*it)->get_text_t(0));
		 glVertex3fv( (GLfloat*) (*it)->vertices[0]->float_data() );

		 glTexCoord2f( (GLfloat) (*it)->get_text_s(1),
			       (GLfloat) (*it)->get_text_t(1));
		 glVertex3fv( (GLfloat*) (*it)->vertices[1]->float_data() );

		 glTexCoord2f( (GLfloat) (*it)->get_text_s(2),
			       (GLfloat) (*it)->get_text_t(2));
		 glVertex3fv( (GLfloat*) (*it)->vertices[2]->float_data() );

		glEnd();
	      }
	    
	    glDisable(GL_TEXTURE_2D);
	  }
	else
	  {
	    // no different colors or last shape
	    if (!settings.f_different_colors_for_shapes || 
		i+1 == number_of_shapes)
	      nr = CYAN; // BLUE;
	    else
	      nr = i+1 == CYAN ? i+2 : i+1;

	    // Set color
	    setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, nr);
	  
	    for (it=triangles->begin(); it != triangles->end(); it++)
	      {
		glLoadName((*it)->name);
		glBegin(GL_TRIANGLES);
		 glNormal3fv( (GLfloat*) (*it)->float_normal() );
		 glVertex3fv( (GLfloat*) (*it)->vertices[0]->float_data() );
		 glVertex3fv( (GLfloat*) (*it)->vertices[1]->float_data() );
		 glVertex3fv( (GLfloat*) (*it)->vertices[2]->float_data() );
		glEnd();
	      }
	  }
    }
}

void displayMeshWithStencil(void)
{
  list<Triangle*>::iterator it;
  list<Triangle*> *triangles;
  int nr;

  glEnable(GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);

  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for (int i=0; i < number_of_shapes; i++)
    {
      triangles=settings.mesh->get_shape(i)->triangles;
	
      if (triangles != NULL)
	{
	  // no different colors or last shape
	  if (!settings.f_different_colors_for_shapes || 
	      i+1 == number_of_shapes)
	    nr = CYAN; // BLUE;
	  else
	    nr = i+1 == CYAN ? i+2 : i+1;

	  setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, nr);

	  for (it=triangles->begin(); it != triangles->end(); it++)
	    {
	      glLoadName((*it)->name);
	      glBegin(GL_TRIANGLES);
	       glVertex3fv( (GLfloat*) (*it)->vertices[0]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[1]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[2]->float_data() );
	      glEnd();

	      glStencilFunc(GL_EQUAL, 0, 1);
	      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	      setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, nr);
	      glBegin(GL_TRIANGLES);
	       glNormal3fv( (GLfloat*) (*it)->float_normal() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[0]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[1]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[2]->float_data() );
	      glEnd();
	      
	      glStencilFunc(GL_ALWAYS, 0, 1);
	      glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
	      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	      setMaterialColor(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, RED);
	      glBegin(GL_TRIANGLES);
	       glNormal3fv( (GLfloat*) (*it)->float_normal() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[0]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[1]->float_data() );
	       glVertex3fv( (GLfloat*) (*it)->vertices[2]->float_data() );
	      glEnd();
	    }
	}
    }

  glDisable(GL_STENCIL_TEST);
}

void tbPointToVector(int x, int y, float v[3])
{
  float d, a;
 
  // project x, y onto a hemi-sphere centered within width, height
  v[0] = (2.0 * x - width()) / width();
  v[1] = (height() - 2.0 * y) / height();
  d = sqrt(v[0] * v[0] + v[1] * v[1]);
  v[2] = cos((3.14159265 / 2.0) * ((d < 1.0) ? d : 1.0));
  a = 1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= a;
  v[1] *= a;
  v[2] *= a;
}

void initLighting(void)
{
  // light from a light source
  const GLfloat diffuseLight[] = {0.4, 0.4, 0.4, 1.0};
  // light from no particulat light source
  const GLfloat ambientLight[] = {0.1, 0.1, 0.1, 1.0};
  // light positions for 4 lights
  const GLfloat lightPositions[4][4] = {{ 1.0,  1.0,  0.0, 0.0},
					{-1.0, -1.0,  0.0, 0.0},
					{-0.1, -0.1,  1.0, 0.0},
					{ 0.1,  0.1, -1.0, 0.0}};

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPositions[0]);
  glEnable(GL_LIGHT0);

  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT1, GL_POSITION, lightPositions[1]);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT2, GL_POSITION, lightPositions[2]);
  glEnable(GL_LIGHT2);

  glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT3, GL_POSITION, lightPositions[3]);
  glEnable(GL_LIGHT3);

  if (settings.f_lighting)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);
}

void setShapeTexture(int j)
{
 // Texture texture;
 // Shape *shape = settings.mesh->get_shape(j);
 // GLint filter = settings.f_texture_filter ? GL_LINEAR : GL_NEAREST;

 // if (shape->get_texture_id() != -1)
 //   {
 //     // we have already one texture for this shape
 //     //  => delete old texture & create a new one
 //     glBindTexture(GL_TEXTURE_2D, 0);
 //     glDeleteTextures(1, &textureList[j]);
 //     glGenTextures(1, &textureList[j]);
 //     shape->set_texture_id(-1);
 //   }

 // const char* texture_name = shape->get_texture_name();

 // if (texture_name != NULL && strlen(texture_name) != 0)
 //   if (texture.read(texture_name) == 0)
 //     {
	//shape->set_texture_id(textureList[j]);
	//glBindTexture(GL_TEXTURE_2D, textureList[j]);
	//
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

	//// Create MipMapped Texture
	////glBindTexture(GL_TEXTURE_2D, texture[2]);
	////glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
	////		    GL_LINEAR);
	////glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
	////		    GL_LINEAR_MIPMAP_NEAREST);
	////gluBuild2DMipmaps(GL_TEXTURE_2D, texture.color_components(),
	////		      texture.width(),texture.height(), 
	////		      texture.format(), GL_UNSIGNED_BYTE,
	////		      texture.data());

	//glTexImage2D(GL_TEXTURE_2D, 0, texture.color_components(),
	//	     texture.width(),texture.height(), 0, texture.format(),
	//	     GL_UNSIGNED_BYTE, texture.data());
	//      
	////glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	////glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
 //     }
}

void initTexture(void)
{
  // create textures, one per shape
  textureList = new GLuint[number_of_shapes];
  glGenTextures(number_of_shapes, textureList);

  for (int j=0; j < number_of_shapes; j++)
    setShapeTexture(j);
}
