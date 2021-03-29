#version 330 compatibility

/* 
 * Date: 2/2/21
 * Description: Transforms vertex coordinates of six quadrilaterals and passes through
 *              to the fragment shader via the rasterizer texture st coordinates.
*/

out vec2 vST;  // texture st coordinates

void
main( )
{

	vST = gl_MultiTexCoord0.st; // texture st coordinates

	//perform model, view and projection transforms
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
