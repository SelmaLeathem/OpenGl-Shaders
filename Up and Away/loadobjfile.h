//Since a lot of includes are similar to sample.cpp need to include at the top

#ifndef LOADOBJFILE_H
#define LOADOBJFILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#include <vector>

#include "Vertex.h"

using std::vector;


// delimiters for parsing the obj file:

#define OBJDELIMS		" \t"


struct Normal
{
	float nx, ny, nz;
};


struct TextureCoord
{
	float s, t, p;
};


struct face
{
	int v, n, t;
};

void	CrossObj( float [3], float [3], float [3] );
char *	ReadRestOfLine( FILE * );
void	ReadObjVTN( char *, int *, int *, int * );
float	UnitObj( float [3] );
float	UnitObj( float [3], float [3] );
int     LoadObjFile( char *name );

#endif