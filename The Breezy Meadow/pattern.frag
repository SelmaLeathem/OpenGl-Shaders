#version 330 compatibility

// Implements the appropriate coloring/lighting and texture per object

uniform vec3  objectColor;		// object color
uniform int objectId;			// Id of object being rendered

uniform sampler2D uTexUnit;

in  vec2  vST;			// texture coords
in  vec3  vN;			// normal vector
in  vec3  vL;			// vector from point to light
in  vec3  vL2;			// vector from point to light
in  vec3  vE;			// vector from point to eye

// use ambient light only for daylight
float   uKa = 0.5; 
float	uKd = 1.0; 
float	uKs = 0.5;

// from internet article with a URL to long to copy
// daylight specular color should be unsaturated yellow
// the ambient color (note not object color) should be the complement
// of this a deep dark purple
vec3  uSpecularColor = vec3(  0.9375, 0.9375, 0.734375 ); // light color

//https://cs.wellesley.edu/~cs307/readings/OpenGL-VRML-Materials.html
// try similar to jade
float   uShininess = 12.8;

void
main( )
{
	vec3 Normal =   normalize(vN);
	vec3 Light     = normalize(vL);
	vec3 Light2     = normalize(vL2);
	vec3 Eye        = normalize(vE);

	// for the object
	vec3 myColor = objectColor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 ambient1;
	vec3 diffuse1;
	vec3 specular1;
	vec3 ambient2;
	vec3 diffuse2;
	vec3 specular2;

	// have two light sources indicated by the suffixes 1 and 2
	 
	ambient1 = uKa * myColor;
	ambient2 = uKa * myColor;

	ambient = ambient1 + ambient2;

	float d = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	diffuse1 = uKd * d * myColor;

	float d2 = max( dot(Normal,Light2), 0. );       // only do diffuse if the light can see the point
	diffuse2 = uKd * d2 * myColor;
	
	diffuse = diffuse1 + diffuse2;
	

	float s = 0.;
		if( dot(Normal,Light) > 0. )	          // only do specular if the light can see the point
		{
			vec3 ref = normalize(  reflect( -Light, Normal )  );
			s = pow( max( dot(Eye,ref),0. ), uShininess );
		}
		specular1 = uKs * s * uSpecularColor;

	float s2 = 0.;
		if( dot(Normal,Light2) > 0. )	          // only do specular if the light can see the point
		{
			vec3 ref2 = normalize(  reflect( -Light2, Normal )  );
			s = pow( max( dot(Eye,ref2),0. ), uShininess );
		}
		specular2 = uKs * s2 * uSpecularColor;

	specular = specular1 + specular2;

	vec3 combinedLight = ambient + diffuse + specular;
	vec3 unitVec = vec3(1.0, 1.0, 1.0);

	vec3 totalLight = min(unitVec, combinedLight);
	
	vec3 newcolor = texture( uTexUnit, vST ).rgb;
	gl_FragColor = vec4( newcolor*(totalLight), 1. );
	
}