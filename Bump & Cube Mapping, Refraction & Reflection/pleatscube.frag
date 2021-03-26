#version 330 compatibility

/* 
 * Name: Selma Leathem
 * Date: 2/2/2021
 * Description: Implements bump mapping using random noise to create creases in the pleats
 *              and implements refraction and reflection.
*/

uniform float uNoiseAmp;        // noise amplitude
uniform float uNoiseFreq;       // noise frequency
uniform sampler3D Noise3;       // the noise texture

uniform float uEta;	//index of refraction
uniform float uMix;	//a value indicating how much refraction/reflection 

uniform samplerCube uReflectUnit;  //the unit the cubemap is in
uniform samplerCube uRefractUnit;  //the unit the cubemap is in

in  vec3  vNs;			// normal vector
in  vec3  vEs;			// vector from point to eye
in  vec3  vMC;  		// vertex coordinates

const vec4  WHITE = vec4( 1.,1.,1.,1. );

// rotate the normal
vec3
RotateNormal( float angx, float angy, vec3 n )
{
        float cx = cos( angx );
        float sx = sin( angx );
        float cy = cos( angy );
        float sy = sin( angy );

        // rotate about x:
        float yp =  n.y*cx - n.z*sx;    // y'
        n.z      =  n.y*sx + n.z*cx;    // z'
        n.y      =  yp;
        // n.x      =  n.x;

        // rotate about y:
        float xp =  n.x*cy + n.z*sy;    // x'
        n.z      = -n.x*sy + n.z*cy;    // z'
        n.x      =  xp;
        // n.y      =  n.y;

        return normalize( n );
}

void
main( )
{
	vec3 Eye = normalize(vEs);  // the eye position vector
	vec3 Normal = normalize(vNs);  // the normal position vector

	//vec4 myColor = uColor;  // background color of the curtain
	
	// get noise values from a texture to create two angles (angx and angy) in which to
	// rotate the normal about
	vec4 nvx = texture( Noise3, uNoiseFreq*vMC );
	float angx = nvx.r + nvx.g + nvx.b + nvx.a  -  2.;
	angx *= uNoiseAmp;

    vec4 nvy = texture( Noise3, uNoiseFreq*vec3(vMC.xy,vMC.z+0.5) );
	float angy = nvy.r + nvy.g + nvy.b + nvy.a  -  2.;
	angy *= uNoiseAmp;
	
	// rotate the normal using the above obtained rotation angles
	Normal = RotateNormal( angx, angy, Normal );
	
	// normalize - eg convert the following into unit vectors
	Normal = normalize(gl_NormalMatrix * Normal);

	//vec3 Light = normalize(vL);
	
	// the eye-reflection vector depends on the normal and eye-position vectors
	vec3 reflectVector = reflect( Eye, Normal);
	// use the reflection vector to select the rgb at a specific (s,t,p) location
	vec4 reflectColor = textureCube( uReflectUnit, reflectVector );

	// the refraction vector also relies on the normal and eye vectors in addition
	// to the index of refraction
	vec3 refractVector = refract( Eye, Normal, uEta );

	vec4 refractColor;
	if( all( equal( refractVector, vec3(0.,0.,0.) ) ) )
	{
		//if the refaction vector returns zero then output reflection instead
		refractColor = reflectColor;
	}
	else
	{
		// use the refraction vector to select the rgb at a specific (s,t,p) location
		refractColor = texture( uRefractUnit, refractVector );

		// mix the refraction color with white to see it more easily
		refractColor = mix( refractColor, WHITE, 0.30 );
	}
	

	gl_FragColor = vec4( mix( refractColor, reflectColor, uMix ).rgb, 1. );

}