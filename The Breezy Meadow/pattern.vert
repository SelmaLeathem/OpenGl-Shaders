#version 330 compatibility

// Animate imported objects and pass them to the fragment shader
// Tree leaves have damped vibratory motion applied to them to mimic shimmering in the wind
// Flowers have damped oscilatory motion applied to mimic blowing in a breeze
// Butterflies have motion applied to mimic flying around with wings flapping
// An apple has motion applied so it falls and rolls while falling then rolls along the ground down a hill


uniform float	t1;		// "Time", from Animate for wind vibrations
uniform float	t2;		// "Time", from Animate for apple falling
uniform float	t4;		// "Time", from Animate for apple falling
uniform float   delta;   //dt/dx, from Animate for apple rolling
uniform float	t5;		// "Time", from Animate for butterfly zigzag
uniform float	t6;		// "Time", from Animate for butterfly steady

uniform bool animation1;  // animation on/off
uniform bool appleMotion; // apple fall on/off

uniform bool appleFall; // apple fall on/off
uniform bool appleRoll; // apple fall on/off

uniform int objectId;  // Id of object being rendered

uniform float flowerDamp; // each flower damps at a different rate
uniform float oscRate; // each flower damps at a different rate
uniform float omegaf; // the amount each flower oscillates by
uniform float tdelay; // the amount each flower oscillates by
 
float ampV = 0.8; // amplitude for wind blasts --- verify don't need
float damp = 1.0f; // 1.5f; // vibration damping
float tStart = 2.0f; //for damping

float xZigZag = -30.f; // for butterfly steady
float zZigZag = 70.f; // for butterfly zigzag

out	 vec2  	vST;	// texture coords
out  vec3  vN;		// normal vector
out  vec3  vL;		// vector from point to light
out  vec3  vL2;		// vector from point to light
out  vec3  vE;		// vector from point to eye

vec3 LightPosition = vec3(  -0.5, 3.5, 0.5 );  //position of light1
vec3 LightPosition2 = vec3(  0.5, 3.5, -0.5 );	//poistion of light2

const float PI = 3.141592654;
float g = 9.8;	// constant of gravitational acceleration

//used to implement == for floating values where instead use if (var > value - epsilon && var < value + epsilon)
float epsilon = 0.01; 

float x0, y0, z0;  // xyz where s or t is zero

float omegaB = PI / 3.f; // omega for oscillations
float omegaV = PI / 5.f; // omega for vibrations 
float omegaR = PI*0.9f;  // omega for rotations


float friction = 1.f; // slows apple rolling down hill
float xL, yL, zL;     //coordinates used to implement apple rolling
float appleRadius = 0.05f;
float appleYorigin = appleRadius; 
float unitFactor = 60.f;  // the magnitude of each unit in the formula describing y(x) for the hill 
float xCoord;  // the variation in the x coordinate as the apple rolls down a hill
float conversionFactor = 0.01333f;  // to convert the hill coordinates to the apple object scale
float hieghtAboveGrass = 0.17f;  // hieght of the grass on the hill

// Implement vibratory motion in x, y and z to mimic leaves shimmering in the wind
// Vibratory motion is described by an equation of the form: 
//  x = x + A*cos(omega*x) + B*sin(omega*x)             (1)
//  where A,B are the amplitudes, omega is the frequency and x can be x, y or z
//  Since there is no memory for x, y, z use the timer as a form of n*dx. In addition
//  since the texture coordinate vST.t varies from 0 to 1 along the leaf from start to tip and it is
//  desired to have the motion of the leaf increase going out from the start of the leaf where
//  it sits on the branch equation (1) is rewritten as:
//  x = x0 + Acos(omega*c*vST.t) + Asin(omega*c*vST.t)   (2)
//  where c varies with x (or rather the system timer).
//
//  Implement damping by observing if the amplitude A in equation 2 is zero then the motion goes back to
//  its original stationary state. Therefore if A varied from A down to zero damping would be mimicked,
//  hence A is changed to 
//   A = A0*exp(-(a-b*t1))                               (3)
//   where t1 is the time from the system timer and a and b are constants.
//
//  To implement c from equation 2 the following is used:
//    c = vert.x*vST.t*t1
//  where the leaf's x coordinate has been introduced to add variety in motion across the tree as a whole as 
//  well as localized variations due to vST.t.

// Vibration for y direction
float yVibration( float y, float st) 
{
	float param = abs(tStart - damp*t1);
	float paramDamp = max(1.f, param);
	float A = ampV*exp(-paramDamp);

	y = y +  A*cos(omegaV*st*t1*y) + A*sin(omegaV*st*t1*y);

   	return y; 
}

// Vibration for x direction
float xVibration(float x,  float st)
{
	float param = abs(tStart - damp*t1);
	float paramDamp = max(1.f, param);
	float A = ampV*exp(-paramDamp);

	x = x +  A*cos(omegaV*st*t1*x) + A*sin(omegaV*st*t1*x);

   	return x; 
}

// Vibration for z direction
float zVibration( float z, float st)
{
   	float param = abs(tStart - damp*t1);
	float paramDamp = max(1.f, param);
	float A = ampV*exp(-paramDamp);

	return z + A*cos(omegaV*st*t1*z) + A*sin(omegaV*st*t1*z);
}



void main( )
{ 
	vST = gl_MultiTexCoord0.st;
	vec3 vert = gl_Vertex.xyz;
	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vN = normalize( gl_NormalMatrix * gl_Normal );	// normal vector
	vL = LightPosition - ECposition.xyz;		// vector from the point
							// to the light position
	vL2 = LightPosition2 - ECposition.xyz;
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							// to the eye position 
	
	// object 0 is a flat patch of grass to which the below equation puts a hill in
	if (objectId == 0){  
		if (vert.x <= 0.f){
			vert.z = vert.z  - 0.0015*(vert.x * vert.x) - 0.4248*vert.x - 0.487;
		}
	}

	// tree leaves shimmer in the wind
	if ( objectId == 2 && animation1 == true )
	{
		vert.x = xVibration( vert.x, vST.t); 
		vert.y = yVibration(vert.y, vST.t); 
		vert.z = zVibration( vert.z, vST.t);	
	}

	// Flowers move in the wind. This is implemented by by having the flowers oscillate
	// back and forth leaning to one side with the motion damping.
	if (animation1 == true && objectId >= 6 && objectId <= 8) 
	{
		float omega = omegaf*PI/5.f; // osciallation frequency
		float xL, zL;  // x, z coordinates
		float dt;   // time unit including damping
		xL = vert.x;
		zL = vert.z;

		if (t1 > tdelay){

			// Implement damping
			dt = oscRate*t6* exp(-abs(flowerDamp*t1));

			// To implment oscillations use the equations which describe a rotation about the 
			// y-axiz and have the time t6 vary between -1 and +1 resulting in back and forth
			// rotations

			vert.x =  xL *cos(omega*dt) + zL*sin(omega*dt); 

			vert.z = -xL*sin(omega*dt) + zL*cos(omega*dt);
		}
			
	}

	// apple falls off tree and rolls down the hill
	else if (objectId == 4 && appleMotion== true )
	{
		if (appleFall){
			  
			// Implement apple rolling
			yL = vert.y -  appleYorigin;
			zL = vert.z - appleRadius;

			// apple rotates about the x axis
			vert.y = appleYorigin + -zL*sin(omegaR*t2) + yL*cos(omegaR*t2);
			vert.z = appleRadius + zL *cos(omegaR*t2) + yL*sin(omegaR*t2); 

			// Implement apple falling due to gravity using standard equation of motion with 
			vert.y = vert.y - 0.5*g*t2*t2;
		}
		else if (appleRoll){
			
			// The only memory is of t2 and not x,y,z latest so need to keep below eqns to
			// maintain the apple's position when it lands on the hill

			yL = vert.y - appleYorigin;
			zL = vert.z - appleRadius;
			vert.y = appleYorigin + -zL*sin(omegaR*t2) + yL*cos(omegaR*t2);
			vert.z = appleRadius + zL *cos(omegaR*t2) + yL*sin(omegaR*t2); 
			vert.y = vert.y - 0.5*g*t2*t2;

			// Implement rolling down the hill with enough friction to cause the motion to stop
			vert.x = vert.x + delta*exp(-friction*delta);
			xCoord= vert.x*unitFactor;

			// The y coordinate makes use of the hill shape determined from a curve fit
			vert.y = vert.y  - conversionFactor*( 0.0015*(xCoord * xCoord) + 0.4248*xCoord + 0.487);
		}
		
	}
	
	// butterflies flap their wings and zigzag across the meadow

	// object's 5 and 9 are two different types of butterfly that zigzag across the meadow in
	// different directions
	// Use standard equation of motion distance = velocity*time for the zigzag movements where
	// xZigZag is the velocity for zigzaging back and forth in the x direction and zZigZag is
	// the velocity for zigzaging in z.

	else if (objectId == 5){  
		y0 = 0.f;
		z0 = 0.f; 

		// flap wings by implempenting rotary motion about the x-axis per dr along the wing
		vert.y = y0 + (vert.y-y0)*sin(omegaB*t4);
		vert.z = z0 + (vert.z-z0)*cos(omegaB*t4);

		// linear zigzag motion
		vert.x = vert.x  - xZigZag*t5;
		vert.z = vert.z + zZigZag*t5;
	}
	else if (objectId == 9){  
		y0 = 0.f;
		x0 = 0.f; 
		
		// flap wings by implempenting rotary motion about the z-axis per dr along the wing
		vert.y = y0 + (vert.y-y0)*sin(omegaB*t4);
		vert.x = x0 + (vert.x-x0)*cos(omegaB*t4);

		// linear zigzag motion
		vert.z = vert.z + zZigZag*t5; 
		vert.x = vert.x - xZigZag*t5; 
	}
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}