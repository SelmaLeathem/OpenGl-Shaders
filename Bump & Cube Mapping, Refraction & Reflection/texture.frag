#version 330 compatibility

/* 
 * Name: Selma Leathem
 * Date: 2/2/21
 * Description: Prints a texture on a quadrilateral surface.
*/

uniform sampler2D TexUnit;  //location of the texture

in vec2 vST; // the texture coordinates

void
main( )
{
	// get the rgb value of the texture at position st
	vec4 newcolor = texture( TexUnit, vST );

	// pass the color through the pipeline
	gl_FragColor = vec4( newcolor.rgb, 1. );
}