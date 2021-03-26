#version 330 compatibility

/* 
 * Name: Selma Leathem
 * Date: 2/2/2021
 * Description: Calculates and passes gl_Position through the pipeline and passes vertex
 *              coordinates to the fragment shader in addition to values needed for per
 *              fragment coloring.
 *              Vertex coordinates are altered to produce pleating in a flat quadrilateral.
*/

uniform float uK;  //Pleat amplitude
uniform float uP;  //Pleat period

out  vec3  vNs;		// normal vector
//out  vec3  vL;	// vector from point to light
out  vec3  vEs;		// vector from point to eye
out  vec3  vMC;  	// vertex coordinates

const float PI = 3.14159265;
const float Y0 = 1.0;


void
main( )
{ 
	vMC = gl_Vertex.xyz;  // vertex coordinates that get exported to the fragment shader
	vec4 vert = gl_Vertex;

	// alter the value of the z coordinate to create pleats in the quadrilateral
	vert.z = vert.z + uK * (Y0- vert.y) * sin( 2.*PI* vert.x/uP );

	// modelView transform to world coordinates
	vec4 ECposition = gl_ModelViewMatrix * vert;
	
	// xTangent is the derivative of the above value for z with respect to x
	float dzdx = uK * (Y0- vert.y) * (2.*PI/uP) * cos( 2.*PI*vert.x/uP );
	
	// yTangent is the derivative of the above value for z with respect to y
	float dzdy = -uK * sin( 2.*PI*vert.x/uP );

	vec3 Tx = vec3(1., 0., dzdx );
	vec3 Ty = vec3(0., 1., dzdy ); 
	
	// the normal to the surface is the cross product of the two tangent vectors Tx and Ty
	vNs = normalize( cross( Tx, Ty ) );
	
	//vL = LightPosition - ECposition.xyz;		// vector from the point
							// to the light position

	vEs = ECposition.xyz - vec3( 0., 0., 0. ) ;		// vector from the point
							// to the eye position 
	
	gl_Position = gl_ModelViewProjectionMatrix * vert;
}
