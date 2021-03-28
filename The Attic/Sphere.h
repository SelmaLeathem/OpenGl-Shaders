/************************************************************************
* Name:        Selma Leathem
* Date:        10/2/2020
* Description: Sphere class definition. The sphere class is able to return
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


#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <cmath>  
#include "Vertex.h"

#define PI 3.141592654

using std::vector;

class Sphere
{
public:
	
	/* Constructor:
	*  Input: sphere radius, 
	*         the number of sectors (east - west)
	*         the number of stacks (north - south)
	*/
	Sphere(float radius, int sectorCount, int stackCount);

	/* Set the cartesian coordinates of the center of the
	*  the sphere. The default is (0,0,0)
	*/
	void setOrigin(float x, float y, float z);

	/* Calculate the vertices and store them in a one-dimensional
	*  vector of type Vertex and size (sectorCount + 1)*(stackCount + 1) 
	*  where the vertices are stored in the following order:
	*  V00, V10, V20 .... Vn0, V01, V11, V21 .... Vn1, V0m..... Vnm
	*  where for each Vij i is the sector and j is the stack.
	*/
	void calculateVertices();

	/* Calculate the normal for each vertex and store in a one-dimension
	 * vector of type Vertex and size (sectorCount + 1)*(stackCount + 1). 
	 * Normals are stored in the same order as the vertices and there is
	 * a one-to-one correspondence with the vertices vector in that
	 * normals[i] corresponds to vertices[i].
	 */
	void calculateNormals();


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
	void calculateIndices();


	/* To return a slice of a sphere, first the starting and ending angles in
	 *  the North-South direction need to be entered */
	void setSliceAngles(float startAngle, float endAngle);

	/* Calculate the Slice Vertices similar as for a whole sphere above, except
	 * for only a slice 
	 * Input:  trimTop is true if trimming the top of the sphere
	 *         trimBottom is true if trimming the bottom of the sphere
	 */
	void calculateSliceVertices(bool trimTop, bool trimBottom);


	/* Calculate the corresponding normals for the above slice
	 * Input:  trimTop is true if trimming the top of the sphere
	 *         trimBottom is true if trimming the bottom of the sphere
	 */
	void calculateSliceNormals(bool trimTop, bool trimBottom);

	/* Get the vertices*/
	vector<Vertex> getVertices();

	/* Get the normals*/
	vector<Vertex> getNormals();

	/* Get the indices of the vertices in such an order that every three
	 * indices form a triangle */
	vector<int> getIndices();

	/* If trim the top of the hemisphere can get the number of indices in
	 * of the triangles that form the top flat surface. This is used in
	 * openGL */
	int getNumSliceTopIndices();

	/* Get Southern most value of the object. Used in OpenGl for stacking
	 * objects */
	float getlastSouthVertex();

	/* Get the radius of the sphere. */
	float getRadius();

private:

	float radius;			// Sphere radius
	Vertex origin;			// Position of sphere center
	
	float startAngle;		// Starting angle for a sphere slice (North-South)
	float endAngle;			// Ending angle for a sphere slice (North-South)

	float lastSouthVertex;  // Most southerly value

	int numIndicesOnTop;	// for a sphere slice the number of indices on the top flat part

	int sectorCount;		// Number of sectors  (east - west)
	int stackCount;			// Number of stacks  (north - south)
	float sectorStep;		// Size of each sector step
	float stackStep;		// Size of each stack step

	vector<Vertex> vertices;	// The vertices of the sphere or slice
	vector<Vertex> normals;		// The normal of each vertex
	vector<int> indices;		// The indices of the vertices such that every three form a triangle

};


#endif



