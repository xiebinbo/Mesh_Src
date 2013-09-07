//
//    File: ply_mesh.h
//
//    (C) 2000 Helmut Cantzler / 2004 Toby Breckon
//
//    Licensed under the terms of the Lesser General Public License.
//

#ifndef _PLY_MESH_H
#define _PLY_MESH_H

#include "mesh.h"

using namespace std;

class PLY_Mesh : public Mesh
{
public:
  int read(FILE *f, int (*update_progress)(int pos),
	   void (*set_total)(int size));
  void write(FILE *f, const char *comment = "");
};

#endif
