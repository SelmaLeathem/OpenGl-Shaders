#version 330 compatibility

// Fragment shader prints a changing pastel spiral on a striped balloon. Both
// the spiral color and deformations are implemented in this file.

uniform float	fragmentTime;		// "Time", from Animate( )

uniform float   uKa, uKd, uKs;		// coefficients of each type of lighting
uniform vec3  uSpecularColor;		// light color
uniform float   uShininess;		// specular exponent

in  vec2  vST;			// texture coords
in  vec3  vN;			// normal vector
in  vec3  vL;			// vector from point to light
in  vec3  vE;			// vector from point to eye

const int NUM_POINTS_SPIRAL = 100; // number of points to draw spiral

const float PI = 3.141592654;

// holds spiral coordinates
vec2 logSpiral1[NUM_POINTS_SPIRAL]; 
vec2 logSpiral2[NUM_POINTS_SPIRAL];


// equation for logarithmic spiral https://www.britannica.com/science/spiral-mathematics
// https://mathworld.wolfram.com/LogarithmicSpiral.html

// Draw two spirals slightly skewed from other 
// ScaleX, ScaleY varies the spiral scaling in the x and y directions, where x and y 
// form the coordinates of a 2d plane
void getLogSpiral(float scaleX, float scaleY) 
{   
	//.3 is largest size of spiral with below fixed parameters
	// these parameters can't be altered to greatly for the spiral to fit on
	// the ballon. They are such as to create a spiral with 2 complete rotations
	int i;
	float a = 0.01, b = 0.11;  //spiral parameters that control the base size and tightness of the spiral
	float dphi = PI / 25.;  // angular amount to rotate by
	float b2 = b + 0.1; // use for the second spiral so it has a slightly different radius for a given angle
	float b2factor = 0.12/(NUM_POINTS_SPIRAL + 1); // use for the second spiral
	vec2 newOrigin = vec2(0.45,0.25);  // origin is fixed for the animation

	// Create the spiral
	for (i = 0; i < NUM_POINTS_SPIRAL; i++) {
		logSpiral1[i].x = scaleX*a * cos(i * dphi) * exp(b * i * dphi);
		logSpiral2[i].x = scaleX*a * cos(i * dphi) * exp((b2 - i*b2factor) * i * dphi);
		logSpiral1[i].y = scaleY*a * sin(i * dphi) * exp(b * i * dphi);
		logSpiral2[i].y = scaleY*a * sin(i * dphi) * exp((b2 - i*b2factor) * i * dphi);	
	}
	
	// Move the origin of the spiral to the center front face of the balloon
	for ( i = 0; i < NUM_POINTS_SPIRAL; i++) {	
		logSpiral1[i].x += newOrigin.x;
		logSpiral2[i].x += newOrigin.x;
		logSpiral1[i].y += newOrigin.y;
		logSpiral2[i].y += newOrigin.y;
	}	
}

// See if the texture coordinate of a corresponding vertex point falls in the spiral 
// and return true if it does
bool inShape( float scaleX, float scaleY){	
	getLogSpiral(scaleX, scaleY); 
	for (int i = 0; i < NUM_POINTS_SPIRAL; i++)
	if ( vST.t > min(logSpiral1[i].y, logSpiral2[i].y) - 0.001 &&
		vST.t < max(logSpiral1[i].y, logSpiral2[i].y) + 0.001 &&
		vST.s> min(logSpiral1[i].x, logSpiral2[i].x) - 0.001 &&
		vST.s < max(logSpiral1[i].x, logSpiral2[i].x + 0.001) ){
					return true;
				}
	return false;
}

void
main( )
{
	vec3 Normal = normalize(vN);
	vec3 Light     = normalize(vL);
	vec3 Eye        = normalize(vE);

	// for the object
	vec3 myColor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float pastelfactor = 0.6; // use to ensure spiral is pastel

	float amplitude = 4.0; // amplitude of spiral

	// scale and positioning of spiral
	float scaleX = amplitude*cos(PI*fragmentTime)*cos(PI*fragmentTime);
	float scaleY = amplitude*cos(PI*fragmentTime*0.95)*cos(PI*fragmentTime*.95);

	// If an s-t point falls within the two spirals give it a pastel color
	// note that the pastel color changes
	if ( inShape(scaleX, scaleY) ){
		
		myColor = vec3( (pastelfactor + scaleX)/(amplitude + pastelfactor), 
						(pastelfactor + scaleX)/(amplitude + pastelfactor), 
						(pastelfactor + scaleY)/(amplitude + pastelfactor));  //variable color

		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );      
		diffuse = uKd * d * myColor;	
	} 

	// color the balloon stripes

	else if( vST.t >= 0.  && vST.t < 0.1 )
	{	
		myColor = vec3(1.,1.,0.);  //yellow
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );      
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.1 && vST.t < 0.2 )
	{
		myColor = vec3(1.,0.,0.);  //red
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );       
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.2 && vST.t < 0.3  )
	{
		myColor = vec3(0.,1.,0.);  //green
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );       
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.3  && vST.t < 0.4 )
	{
		myColor = vec3(0.,0.,1.);  //blue
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );       
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.4 &&  vST.t < 0.5 )
	{
		myColor = vec3(0.99, 0.2695, 0.0);  // orange
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );      
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.5 &&  vST.t < 0.6 )
	{
		myColor = vec3(1., 1., 1.);  //white
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );       
		diffuse = uKd * d * myColor;	
	}
	else if( vST.t >= 0.6 &&  vST.t < 0.7 )
	{
		myColor = vec3(0.,1.,1.);  //cyan
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );     
		diffuse = uKd * d * myColor;	
	}

	// color the balloon basket brown 
	else  // if( vST.t >= 0.7 &&  vST.t < 1. )
	{
		myColor = vec3(0.396, 0.263, 0.129);  //brown
		ambient = uKa * myColor;

		float d = max( dot(Normal,Light), 0. );      
		diffuse = uKd * d * myColor;	
	}
	

	float s = 0.;
		if( dot(Normal,Light) > 0. )	         
		{
			vec3 ref = normalize(  reflect( -Light, Normal )  );
			s = pow( max( dot(Eye,ref),0. ), uShininess );
		}
		specular = uKs * s * uSpecularColor;
	
	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
	
}