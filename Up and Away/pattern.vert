#version 330 compatibility

// Vertex shader passes vectors to the fragment shader and implements movements
// in the xyz vertex coordinates of the object

uniform float	vertexTime;		// "Time", from Animate( )

out	 vec2  	vST;		// texture coords
out  vec3  vN;		// normal vector
out  vec3  vL;		// vector from point to light
out  vec3  vE;		// vector from point to eye

vec3 LightPosition = vec3(  0., 1., 2.5 );	// position of the light source

const float PI = 	3.141592654;

// The balloon is oriented so its z-axis is North

//z vertex values of the top/bottom of the balloon part excluding the basket
const float bottomOfBalloon = 659;
const float topOfBalloon = 3272;

float amplitude = 100.0;  //wave amplitude of the basket 
float amplitudeBalloon = 300.0; // wave amplitude of the balloon

void main( )
{ 
	int signX = 1;   //sign of the x vertex component
	int signY = 1;	//sign of the y vertex component
	float distortX, distortY;  //the amount to alter the xy vertices
	float radius;  // radius of the current vertex from object center
	float phase;  //phase angle for current vertex

	vST = gl_MultiTexCoord0.st;
	vec3 vert = gl_Vertex.xyz;
	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vN = normalize( gl_NormalMatrix * gl_Normal );	// normal vector
	vL = LightPosition - ECposition.xyz;		// vector from the point
							// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							// to the eye position 
	
	// get the sign of the vertex coordinates
	if (vert.x < 0.)
		signX = -1;
	if (vert.y < 0.)
		signY = -1;

	// get the angle of the coordinate from the x = 0 axis
	radius = sqrt(vert.x*vert.x + vert.y*vert.y);
	if (radius > 0.001)
		phase = acos(vert.x/radius);
	else
		phase = 0;

	// amounts to distort the x and y coordinates by
	distortX = sin(PI*vertexTime);
	distortY = sin(PI*vertexTime*1.1);
	
	// The x and y coordinates keep the same sign and only expand. They don't shrink/decrease
	// in any way
	if (vert.z > bottomOfBalloon && vert.z < topOfBalloon){  
		amplitude = amplitudeBalloon;
	}
	
	// Distort the balloon in (x,y)
	vert.x = vert.x + signX*amplitude*distortX*distortX;
	vert.y = vert.y + signY*amplitude*distortY*distortY;

	// The z direction pulls part of the object up/down depending on the phase angle of the vertex
	vert.z = vert.z + amplitude*cos(phase + PI*vertexTime)*sin(PI*vertexTime*0.8)*sin(PI*vertexTime*0.8);
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}