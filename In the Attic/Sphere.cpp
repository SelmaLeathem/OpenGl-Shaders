/************************************************************************
* Name:        Selma Leathem
* Date:        10/2/2020
* Description: Sphere class implemenation. The sphere class is able to return
*              the vertices and normals of points on a sphere as one-dimensional
*              vectors (see function descriptions for more information) and the  
*              indices of the cooresponding vertices in the vector that form 
*              triangles for use with OpenGl to draw the sphere. eg:
* 
*              for the first triangle:
* 
*             verticies[indices[0]] -> verticies[indices[1]] -> verticies[indices[2]]
* 
*             verticies[indices[0]] *------------------* verticies[indices[2]]
*                                   |       
*                                   |     
*             verticies[indices[1]] * 
* 
*              where the diagonal edge has not been drawn in. These triangles 
*              collectively form a sphere and are used by OpenGL to draw a sphere.
*             
*              Sphere angle ranges: 
*                      PI/2 to -PI/2 from North to South directions
*                      0 to 2*PI from West to East
* 
*              A sphere has (countStack + 1) points in the North-South direction and
*              (countSegments + 1) points in the East-West direction.
* 
*              In addition, vertices, normals and indices can be returned
*              for slices out of the sphere. For sphere slices the user
*              enters the starting and ending angles.
* 
*              References include:
* 
* Spherical Coordinates: https://mathworld.wolfram.com/SphericalCoordinates.html
* How make sphere in openGl: http://www.songho.ca/opengl/gl_sphere.html  
* Computer Graphics Programming in OpenGL Using C++ by V Scott Gordon and John L Clevenger
* OSUsphere.cpp
* 
*/

#include "sphere.h"

/* Constructor:
*  Input: sphere radius, 
*         the number of sectors (east - west)
*         the number of stacks (north - south)
*/
Sphere::Sphere(float radius, int sectorCount, int stackCount)
{
	
	this->radius = radius;
	this->sectorCount = sectorCount;
	this->stackCount = stackCount;

	sectorStep = 2.0f * (float)PI / sectorCount;
	stackStep = (float)PI / stackCount;

	// Initialize variables

	this->origin.x = 0.0f;
	this->origin.y = 0.0f;
	this->origin.z = 0.0f;

	this->startAngle = 0.0f;
	this->endAngle = 0.0f;

	this->numIndicesOnTop = 0;

	this->lastSouthVertex = 0.0f;
}

/* Set the cartesian coordinates of the center of the
*  the sphere. The default is (0,0,0)
*/
void Sphere::setOrigin(float x, float y, float z)
{
	this->origin.x = x;
	this->origin.y = y;
	this->origin.z = z;
}

/* To return a slice of a sphere, first the starting and ending angles in
*  the North-South direction need to be entered */
void Sphere::setSliceAngles(float startAngle, float endAngle)
{
	this->startAngle = startAngle;
	this->endAngle = endAngle;
}

/* Calculate the normal for each vertex and store in a one-dimension
* vector of type Vertex and size (sectorCount + 1)*(stackCount + 1). 
* Normals are stored in the same order as the vertices and there is
* a one-to-one correspondence with the vertices vector in that
* normals[i] corresponds to vertices[i].
*/
void Sphere::calculateVertices()
{
	/* y points north
	 * stack angle is 90 to -90 (north - south)
	 * sector angle is 0 to 360 ( east - west)
	 */
	float sectorAngle, stackAngle, 
			stackPosition;	// Position of the vertex along the stack
	Vertex vertex;  		// the vertex of the point calculated

	for (int i = 0; i <= stackCount; i++)
	{
		stackAngle = (float)PI/2.0f - i*stackStep;
		vertex.y = radius * sinf(stackAngle) + origin.y;
		stackPosition = radius * cosf(stackAngle);

		for (int j = 0; j <= sectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			vertex.x = stackPosition * sinf(sectorAngle) + origin.x;
			vertex.z = stackPosition * cosf(sectorAngle) + origin.z;

			vertices.push_back(vertex);
			
		}
	}

	this->lastSouthVertex = vertices[(vertices.size() - 1)].y;
}

/* Calculate the Slice Vertices similar as for a whole sphere above, except
* for only a slice 
* Input:  trimTop is true if trimming the top of the sphere
*         trimBottom is true if trimming the bottom of the sphere
*/
void Sphere::calculateSliceVertices(bool trimTop, bool trimBottom)
{
	/* y points north
	 * stack angle is 90 to -90 (north - south)
	 * sector angle is 0 to 360 ( east - west)
	 */
	float sectorAngle, stackAngle, 
			stackPosition;		// Position of the vertex along the stack
	Vertex vertex;				// the vertex of the point calculated

	if (!trimTop)
		startAngle = (float)PI / 2.0f;
	if (!trimBottom)
		endAngle = -(float)PI / 2.0f;

	stackStep = (startAngle - endAngle) / stackCount;

	for (int i = 0; i <= stackCount; i++)
	{
		stackAngle = startAngle - i * stackStep;
		vertex.y = radius * sinf(stackAngle) + origin.y;
		stackPosition = radius * cosf(stackAngle);

		for (int j = 0; j <= sectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			// Put the vertex points in the center of the plane cutting
			// y-location of where trimmed instead of along the sphere
			// perimeter. Then when forming triangles the triangles at
			// this location will be on a two-dimensional surface similar
			// to the top/bottom of a cylinder.
			if ( trimTop &&  i == 0)
			{
				vertex.x = 0.0f + origin.x;
				vertex.z = 0.0f + origin.z;
			}
			else if ( trimBottom && i == stackCount)
			{
				vertex.x = 0.0f + origin.x;
				vertex.z = 0.0f + origin.z;
			}
			else
			{
				vertex.x = stackPosition * sinf(sectorAngle) + origin.x;
				vertex.z = stackPosition * cosf(sectorAngle) + origin.z;
			}
			
			vertices.push_back(vertex);

		}
	}

	// return the position of the southern most y-coordinate 
	this->lastSouthVertex = vertices[(vertices.size() - 1)].y;
}

/* Calculate the normal for each vertex and store in a one-dimension
* vector of type Vertex and size (sectorCount + 1)*(stackCount + 1). 
* Normals are stored in the same order as the vertices and there is
* a one-to-one correspondence with the vertices vector in that
* normals[i] corresponds to vertices[i].
*/
void Sphere::calculateNormals()
{
	// All listed references above stated how to get the normals
	// There is one normal per vertex point

	int numVertices = (int)vertices.size();
	float radiusInverse = 1.0f / radius;
	Vertex normal;
	
	// If there are no vertices then calculate them
	if (vertices.size() == 0)
	{
		calculateVertices();
	}

	for (int i = 0; i < numVertices; i++)
	{
		normal.x = radiusInverse * vertices[i].x;
		normal.y = radiusInverse * vertices[i].y;
		normal.z = radiusInverse * vertices[i].z;
		normals.push_back(normal);
	}
}

/* Calculate the corresponding normals for the above slice
*  Input:  trimTop is true if trimming the top of the sphere
*         trimBottom is true if trimming the bottom of the sphere
*/
void Sphere::calculateSliceNormals(bool trimTop, bool trimBottom)
{
	// All listed references above stated how to get the normals
	// There is one normal per vertex point

	int numVertices = (int)vertices.size();
	float radiusInverse = 1.0f / radius;
	Vertex normal;

	// If there are no vertices then calculate them
	if (vertices.size() == 0)
	{
		calculateVertices();
	}

	for (int i = 0; i < numVertices; i++)
	{
		normal.x = radiusInverse * vertices[i].x;
		normal.y = radiusInverse * vertices[i].y;
		normal.z = radiusInverse * vertices[i].z;

		// Ensure the normals on the top and bottom surfaces point 
		// outward from the plane of where sliced
		if (trimTop && i <= sectorCount)
		{
			normal.y = fabs(normal.y);
		}
		else if (trimBottom && i >= stackCount * (sectorCount + 1))
		{
			normal.y = -fabs(normal.y);
		}
		
		normals.push_back(normal);
	}
}

/* Stores the index of the sphere vertices in an order such that
*  the first three indices are the index positions of the vertices in
*  the verticies array that form triangle 0 on the sphere and the 
*  next three indices in the indicies array coorespond to the next
*  three vertices for triangle 1 and so on forming a sphere of triangles
*  eg for triangle 0:
*      verticies[indices[0]] *------------------* verticies[indices[2]]
*                             |       
*                             |     
*      verticies[indices[1]]  * 
*      
*      the three vertices form a triangle, and note the diagonal edge
*      is left out of the the diagram.
*/
void Sphere::calculateIndices()
{
	// references: 
	// http://www.songho.ca/opengl/gl_sphere.html 
	// Computer Graphics Programming in OpenGL Using C++ by V Scott Gordon and John L Clevenger
	int index1, index2;

	for (int i = 0; i < stackCount; i++)
	{
		index1 = i * (sectorCount + 1);
		index2 = index1 + sectorCount + 1;

		/* Every two triangles form a rectangle:
		      p1 *    * p2
			  p3 *    * p4

			  triangle 1 is: p1 -> p3 -> p2
			  triangle 2 is: p2 -> p3 -> p4

		    Points p1 and p2 share the same stack value.
			Points p3 and p4 share the stack value one level below.
		 */

		for (int j = 0; j < sectorCount; ++j)
		{
			// Ensures the top stack of the sphere is made up of single
			// triangles that converge on a single point
			// Only triangle 2 is used.
			if (i != 0)
			{
				indices.push_back(index1 + j);
				indices.push_back(index2 + j);
				indices.push_back(index1 + 1 + j);
			}
			
			// Ensures the bottom stack of the sphere is made up of single
			// triangles that converge on a single point
			// Only triangle 1 is used.
			if (i != (stackCount - 1))
			{
				indices.push_back(index1 + 1 + j);
				indices.push_back(index2 + j);
				indices.push_back(index2 + 1 + j);

			}
		}

		// Get the number of indices used for the top part. This is useful
		// for slices to separately color the top of the slice in openGL.
		if (i == 0)
		{
			numIndicesOnTop = (int)indices.size();
		}
	}
}

/* Get the vertices*/
vector<Vertex> Sphere::getVertices()
{
	return vertices;
}

/* Get the normals*/
vector<Vertex> Sphere::getNormals()
{
	return normals;
}

/* Get the indices of the vertices in such an order that every three
* indices form a triangle */
vector<int> Sphere::getIndices()
{
	return indices;
}

/* If trim the top of the hemisphere can get the number of indices in
* of the triangles that form the top flat surface. This is used in
* openGL */
int Sphere::getNumSliceTopIndices()
{
	return numIndicesOnTop;
}

/* Get Southern most value of the object. Used in OpenGl for stacking
* objects */
float Sphere::getlastSouthVertex()
{
	return lastSouthVertex;
}

/* Get the radius of the sphere. */
float Sphere::getRadius()
{
	return radius;
}
