/*
*  Name:        Selma Leathem
*  Date:        10/22/2020
*  Description: Added stationary and moving light sources along with several objects.
*               Lighting can be switched on/off using keyboard keys:
*               f,F = freeze/unfreeze motion
*               1 = turn off/on ambient light in plus XZ sector
*               2 = turn off/on ambient light in minus XZ sector
*               l,L = turn off/on lamp light
*               t,T = turn off/on train light
*               r,R = turn off/on red spot lights
*/

#include "loadobjfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#include "utility.h"
#include "Sphere.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

struct VertexG {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Joe Graphics" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 800 };

// size of the 3d box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 2. };
const GLfloat AXES_LENGTH = { 3.5 };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };

// ADDED HERE 
//GLOBAL FOR NOW
float White[] = { 1.,1.,1.,1. };


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong

//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER

// should we turn the shadows on?

//#define ENABLE_SHADOWS



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoShadowMenu();
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
unsigned char *	BmpToTexture( char *, int *, int * );
void			HsvRgb( float[3], float [3] );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);

// for turning textures on/off
void DoTextureMenu(int);
int textureValue = 1;  //default is on

// for switching to a large white vase to demonstrate the spotlights
void DoSwitchVase(int);
int vaseValue = 0;

// use to hold sphere coordinates
struct SphereObject {
	int numIndices;
	vector<Vertex> vertices, normals;
	vector<int> indices;
};

// use for texture
GLuint TexVase, TexDesk, TexBear; // global variables


#define MS_PER_CYCLE 50000
float Time = 0.f;
int stopTime = 0;
int startTime = 0;

int Frozen = 1;  //default is on
int redSpotOn = 1; //default is on
int lampOn = 1; //default is on
int trainLightOn = 1; //default is on 
int ambientLightPlusXOn = 1; //default is on
int ambientLightminusXOn = 1; //default is on

// object display lists
GLuint	LampPostList;				
GLuint  LampLightList;
GLuint	StatueList;
GLuint	TrainList;
GLuint  SphereList;  //flat shaded surfaced sphere made with triangles
GLuint	DeskList;
GLuint	TeddyList;
GLuint	VaseList;
GLuint	ShinyVaseList;
GLuint	ShinyVaseSpotLightList;
GLuint	TeddyEyeLight1;
GLuint	TeddyEyeLight2;

// Positions of objects and light sources
VertexG lampostPosition, statuePosition, trainPosition, deskPosition, teddyPosition, vasePosition, shinyVasePosition;
VertexG lampLightPosition, teddyEyePosition1, teddyEyePosition2;

// set the glMaterialfv, and glMaterialf parameters
void SetMaterial(float r, float g, float b, float shininess);

//overloaded setmaterial
void SetMaterial(float* diffuse, float* ambient, float* specular, float shininess);

// set the point light up 
void SetPointLight(int ilight, float x, float y, float z, float r, float g, float b);

// set the spot light up
void SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b);

// set up the ambient light 
void SetAmbientLight(int ilight, float x, float y, float z, float r, float g, float b);


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never returns
	// this line is here to make the compiler happy:

	return 0;
}

// set the material lighting properties
void SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


// overloaded: set the material lighting properties
void SetMaterial(float* diffuse, float* ambient, float* specular, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// set the point light up
void SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}


void SetAmbientLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Array3(r, g, b));
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(ilight, GL_AMBIENT, Array3(r, g, b));
	glLightfv(ilight, GL_DIFFUSE, Array3(1.f, 1.f, 1.f));
	glLightfv(ilight, GL_SPECULAR, Array3(1.f, 1.f, 1.f));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 1.);
	glLightfv(ilight, GL_POSITION, Array3(x, y , z));
	glEnable(ilight);
}

// set the spot light up
void SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	//glLightf(ilight, GL_SPOT_CUTOFF, 45.); // original value

	// narrowed the angular range before drop off
	glLightf(ilight, GL_SPOT_CUTOFF, 20.);
	
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	int interval = startTime - stopTime;
	int ms = glutGet(GLUT_ELAPSED_TIME) - interval;
	ms %= MS_PER_CYCLE;
	Time = (float)ms / ((float)MS_PER_CYCLE/7.f); // [0.,6.)

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	// References outside of class lectures and notes
	// Book Computer Graphics Programming in OpenGL Using C++ by Gordon and Clevenger
	//ambient white levels reference: http://math.hws.edu/graphicsbook/c4/s2.html
	// complete example: https://users.cs.jmu.edu/bernstdh/web/common/lectures/summary_opengl-lighting.php
	// complete example with moving lights: https://www.glprogramming.com/red/chapter05.html


	//colors for different materials: https://cs.wellesley.edu/~cs307/readings/OpenGL-VRML-Materials.html
	float goldAmbient[4] = { 0.2473f, 0.1995f, 0.0745f, 1.0f };
	float goldDiffuse[4] = { 0.7516f, 0.6065f, 0.2265f, 1.0f };
	float goldSpecular[4] = { 0.6283f, 0.5559f, 0.3661f, 1.0f };
	float goldShininess = 51.2f;

	float ironAmbient[4] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
	float ironDiffuse[4] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
	float ironSpecular[4] = { 0.332741f, 0.328634f, 0.346435f, 1.0f };
	float ironShininess = 38.4f; //might need to decrease

	float brassAmbient[4] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
	float brassDiffuse[4] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
	float brassSpecular[4] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
	float brassShininess = 27.89f; //might need to decrease

	float bronzeAmbient[4] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
	float bronzeDiffuse[4] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
	float bronzeSpecular[4] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
	float bronzeShininess = 25.6f; //might need to decrease

	float perlAmbient[4] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
	float perlDiffuse[4] = { 1.f, 0.829f, 0.829f, 1.0f };
	float perlSpecular[4] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
	float perlShininess = 27.89f; //might need to decrease

	float silverAmbient[4] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
	float silverDiffuse[4] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
	float silverSpecular[4] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
	float silverShininess = 51.2f; //might need to decrease

	float shinyWhiteAmbient[4] = { 0.f, 0.f, 0.f, 1.0f };
	float shinyWhiteDiffuse[4] = { 0.55f, 0.55f, 0.55f, 1.0f };
	float shinyWhiteSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float whiteShininess = 32.f; //might need to decrease

	float chromeAmbient[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
	float chromeDiffuse[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float chromeSpecular[4] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
	float chromeShininess = 76.8f; //might need to decrease

	float whiteRubberAmbient[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	float whiteRubberDiffuse[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	float whiteRubberSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float whiteRubberShininess = 10.f; //might need to decrease

	// lightpost and trainlight positions relative to geometic features of the geomtry
	GLfloat lightpost, trainLight;

	// origin location of the spot light
	float spotlight1Origin[3] = { teddyEyePosition1.x, 0.f, teddyEyePosition1.z };
	float spotlight2Origin[3] = { teddyEyePosition2.x, 0.f, teddyEyePosition2.z };
	
	// direction of the spot light from its origin
	float spot1Direction[3] = {  vasePosition.x - spotlight1Origin[0] -0.2f, 0.f, vasePosition.z - spotlight1Origin[2] };
	float spot2Direction[3] = { shinyVasePosition.x - spotlight2Origin[0], 0.f, shinyVasePosition.z - spotlight2Origin[2] + .45f };

	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif

	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// if we do this, then the light will be wrt the scene at XLIGHT, YLIGHT, ZLIGHT:
	//glLightfv(GL_LIGHT0, GL_POSITION, Array3(0.f, 2.0f, 2.f));


	// set the eye position, look-at position, and up-vector:

	gluLookAt( 0., 0., AXES_LENGTH + 1.0,     0., 0., 0.,     0., 1., 0. );

	// if we do this, then the light will be wrt the eye at XLIGHT, YLIGHT, ZLIGHT:   //ADDED HERE
	// glLightfv( GL_LIGHT0, GL_POSITION, lightPosition) );

	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );


	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure normals get unitized:
	glEnable(GL_NORMALIZE);

	//ADD HERE, SO LIGHT MOVES WITH ROTATION OF THE AXES AFTER ROTATE AXES

	//---- Set light Sources Here ----
	
	// Put glColor3f objects here before enable lighting
	
	// Lamp Light
	SetPointLight(GL_LIGHT0, lampLightPosition.x, lampLightPosition.y - 0.3, lampLightPosition.z, 0.8f, .388f, 0.f);
	glPushMatrix();
	glColor3f(0.8f, .388f, 0.f);
	glCallList(LampLightList);
	glPopMatrix();
	// Moving train headlights

	// Headlight 1
	trainLight = 2.3f; // light position relative to train center
	SetPointLight(GL_LIGHT1, trainPosition.x + trainLight + Time, 0.8f, trainPosition.z - .2f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(trainPosition.x + trainLight + Time, trainPosition.y + 0.5f, trainPosition.z + .25f);
	glColor3f(.973, .961, .92);
	glutSolidSphere(0.08, 20, 20);
	glPopMatrix();

	// Headlight 2
	SetPointLight(GL_LIGHT2, trainPosition.x + trainLight + Time, 0.8f, trainPosition.z + .2f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(trainPosition.x + trainLight + Time, trainPosition.y + 0.5f, trainPosition.z - .25f);
	glColor3f(1.f, 1.f, 1.f);
	glutSolidSphere(0.08, 20, 20);
	glPopMatrix();
	

	/* ambient light add further lighting where needed for darker objects*/
	
	// ambient light over lamppost
	SetAmbientLight(GL_LIGHT3, lampostPosition.x + .2f, lampostPosition.y + 0.1f, lampostPosition.z+0.3f, .02f, .02f, .02f);
	
	
	// ambient light over desk
	SetAmbientLight(GL_LIGHT4, deskPosition.x + .3f, deskPosition.y + 0.5f, deskPosition.z, .02f, .02f, .02f);

	// ambient light for dark
	SetAmbientLight(GL_LIGHT7, 0.f, 0.5f, 1.f, .01f, .01f, .01f);
	

	// two red spot lights are the teddy bear's eyes
	
	// eye 1 -- left eye
	SetSpotLight(GL_LIGHT5, teddyEyePosition1.x, teddyEyePosition1.y, teddyEyePosition1.z,
		spot1Direction[0], spot1Direction[1], spot1Direction[2], 1.0f, 0.f, 0.f);
	glCallList(TeddyEyeLight1);

	// eye 2 -- right eye
	SetSpotLight(GL_LIGHT6, teddyEyePosition2.x, teddyEyePosition2.y, teddyEyePosition2.z,
		spot1Direction[0], spot2Direction[1], spot2Direction[2], 1.0f, 0.f, 0.f);
	glCallList(TeddyEyeLight2);
	
	// enable/disable lights depending on global values that are toggled with keyboard keys
	(lampOn) ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);
	(trainLightOn) ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
	(trainLightOn) ? glEnable(GL_LIGHT2) : glDisable(GL_LIGHT2);
	(ambientLightPlusXOn) ? glEnable(GL_LIGHT3) : glDisable(GL_LIGHT3);
	(ambientLightminusXOn) ? glEnable(GL_LIGHT4) : glDisable(GL_LIGHT4);
	(redSpotOn) ? glEnable(GL_LIGHT5) : glDisable(GL_LIGHT5);
	(redSpotOn) ? glEnable(GL_LIGHT6) : glDisable(GL_LIGHT6);

	glEnable(GL_LIGHTING);  //use lightmodel going forward instead of color3f
	
	/* ---- insert Objects Here --- */

	// when draw the object need to specify the back of the object by reversing the order of the vertices
	// at that side/place to clock-wise
	// front of the object is counter-clockwise
	// draw the current object

	// insert sphere here 
	glShadeModel(GL_FLAT);  //sphere uses the flat shading model
	SetMaterial(goldDiffuse, goldAmbient, goldSpecular, goldShininess);
	glCallList(SphereList);

	// Goint forward the rest of objects use GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	// Lamp Post
	SetMaterial(bronzeDiffuse, bronzeAmbient, bronzeSpecular, bronzeShininess);
	glCallList(LampPostList);

	// Statue
	SetMaterial(perlDiffuse, perlAmbient, perlSpecular, perlShininess);
	glCallList(StatueList);

	// Moving train
	SetMaterial(ironDiffuse, ironAmbient, ironSpecular, ironShininess);
	glPushMatrix();
	glTranslatef(trainPosition.x +Time, trainPosition.y, trainPosition.z);
	glCallList(TrainList);
	glPopMatrix();

	// Shiny Vase -- Switch to a large white vase using the menu to demonstrate the red spotlights
	if (!vaseValue) {
		SetMaterial(brassDiffuse, brassAmbient, brassSpecular, brassShininess);
		glCallList(ShinyVaseList);
	}
	else {
		SetMaterial(shinyWhiteDiffuse, shinyWhiteAmbient, shinyWhiteSpecular, whiteShininess);
		glCallList(ShinyVaseSpotLightList);
	}

	if (textureValue)  // Turn textures on/off using the menu
	{
		glEnable(GL_TEXTURE_2D);
	}
		
	// Porcelain Vase
	glBindTexture(GL_TEXTURE_2D, TexVase); // might not need a secondtime
	SetMaterial(shinyWhiteDiffuse, shinyWhiteAmbient, shinyWhiteSpecular, whiteShininess);
	glCallList(VaseList);
		
	// Desk
	glBindTexture(GL_TEXTURE_2D, TexDesk); // might not need a secondtime
	SetMaterial(goldDiffuse, goldAmbient, goldSpecular, goldShininess);
	glCallList(DeskList);

	// Teddy Bear	
	glBindTexture(GL_TEXTURE_2D, TexBear); // might not need a secondtime
	SetMaterial(goldDiffuse, goldAmbient, goldSpecular, goldShininess);
	glCallList(TeddyList);

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);  

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:

	
	glDisable( GL_DEPTH_TEST );
	glColor3f( 1., 1., 1. );
	DoRasterString( 0.5, 3.8, 0., (char *)"In the Attic" );
	

	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	/*
	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., (char *)"Text That Doesn't" );
	*/

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}

// setup a texture on/off menu item
void	DoTextureMenu(int id)
{
	textureValue = id; //default is on

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// setup a switch vase menu item
void	DoSwitchVase(int id)
{
	vaseValue = id; //default is on

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoShadowsMenu(int id)
{
	ShadowsOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int vasemenu = glutCreateMenu(DoSwitchVase);
	glutAddMenuEntry("Brass Vase", 0);
	glutAddMenuEntry("Large White Vase", 1);

	int texturemenu = glutCreateMenu(DoTextureMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int shadowsmenu = glutCreateMenu(DoShadowsMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Texture",      texturemenu);
	glutAddSubMenu(   "Vase",          vasemenu);
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );

#ifdef ENABLE_SHADOWS
	glutAddSubMenu(   "Shadows",       shadowsmenu);
#endif

	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	char* filenameVase = (char*)"images/porcelain.bmp";  // texture file
	char* filenameDesk = (char*)"images/wood.bmp";  // texture file
	char* filenameBear = (char*)"images/teddyFur2.bmp";  // texture file

	//char* filename = "images/porcelain.bmp"; //(char*)"squarefishColor.bmp";  // texture file

	int width = 1024, height = 1024;  // file dimensions
	int level = 0, ncomps = 3, border = 0;  // variables used to convert the image to a texture
	unsigned char* TextureVase, * TextureDesk, * TextureBear; // *Texture; // the resulting texture
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( NULL);  //default starts out as on

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// Get and Setup the Textures

	// get the texture
	
	TextureVase = BmpToTexture(filenameVase, &width, &height);
	TextureDesk = BmpToTexture(filenameDesk, &width, &height);
	TextureBear = BmpToTexture(filenameBear, &width, &height);

	//Set up the textures such as wraping, binding handles etc..

	//page 19 in slide
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &TexVase); // assign binding handles
	glGenTextures(1, &TexDesk); // assign binding handles
	glGenTextures(1, &TexBear); // assign binding handles


	// Set up the Vase Texture
	glBindTexture(GL_TEXTURE_2D, TexVase); // make the Tex0 texture current
										// and set its parameters
	// define how to wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// define texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// tell openGL what to do with texel colors
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, level, ncomps, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, TextureVase);
	

	
	// Set up the desk texture
	glBindTexture(GL_TEXTURE_2D, TexDesk); // make the Tex0 texture current
									// and set its parameters
// define how to wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// define texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// tell openGL what to do with texel colors
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, level, ncomps, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, TextureDesk);

	// Set up the bear texture
	glBindTexture(GL_TEXTURE_2D, TexBear); // make the Tex0 texture current
									// and set its parameters
// define how to wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// define texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// tell openGL what to do with texel colors
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, level, ncomps, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, TextureBear);
	
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	int loadobjReturn;  // return value from loading object file
	GLfloat lightpost;  // holds lightpost geometric information

	// names of object files
	char* fileNameLampost = "objects/LampPost.obj";
	char* fileNameStatue = "objects/statueFemale.obj";
	char* fileNameTrain = "objects/railRoadTankCar.obj";
	char* fileNameDesk = "objects/desk.obj";
	char* fileNameTeddy = "objects/teddy.obj";
	char* fileNameVase = "objects/vase.obj";
	char* fileNameShinyVase = "objects/vaseShiny.obj";

	// Sphere properties of flat shaded sphere
	SphereObject sphereObj;
	float radius = 0.3;
	int sectorCount = 15, stackCount = 15, index;
	Sphere sphere(radius, sectorCount, stackCount);
	sphere.calculateVertices();
	sphereObj.vertices = sphere.getVertices();
	sphere.calculateIndices();
	sphereObj.indices = sphere.getIndices();
	sphere.calculateNormals();
	sphereObj.normals = sphere.getNormals();
	sphereObj.numIndices = sphereObj.indices.size();

	// Define  the lamp post position
	lampostPosition.x = 0.8f;
	lampostPosition.y = 0.f;
	lampostPosition.z = 1.7f;

	// Define  the lamp post light position
	lightpost = 10.f * 0.05 * .707;  
	lampLightPosition.x = lampostPosition.x - lightpost;
	lampLightPosition.y = 1.75f;
	lampLightPosition.z = lampostPosition.z - lightpost;

	// Define the statue position
	statuePosition.x = 2.5f; // 1.5f;
	statuePosition.y = 0.f;
	statuePosition.z = 0.2f;

	// Define the train position
	trainPosition.x = -5.f;
	trainPosition.y = 0.f; 
	trainPosition.z = -1.2f;

	// Define the desk position
	deskPosition.x = -2.f;
	deskPosition.y = 0.f; 
	deskPosition.z = 1.3f;

	// Define the teddy bear position
	teddyPosition.x = deskPosition.x + 0.5f;
	teddyPosition.y = 0.0f;
	teddyPosition.z = 1.3f;

	// Define the teddy bear left eye position
	teddyEyePosition1.x = teddyPosition.x + .038;
	teddyEyePosition1.y = .54f;
	teddyEyePosition1.z = teddyPosition.z -0.05f; //plus or minus delta-z for each eye

	// Define the teddy bear right eye position
	teddyEyePosition2.x = teddyEyePosition1.x;
	teddyEyePosition2.y = teddyEyePosition1.y;
	teddyEyePosition2.z = teddyPosition.z + 0.05f; //plus or minus delta-z for each eye

	// Define the porcelain vase position
	vasePosition.x = deskPosition.x + 1.0f;
	vasePosition.y = 0.5f;
	vasePosition.z = 0.8f;

	// Define the shiny vase position
	shinyVasePosition.x = 0.0f;
	shinyVasePosition.y = 0.0f;
	shinyVasePosition.z = 0.8f;


	glutSetWindow( MainWindow );


	// flat shaded sphere
	SphereList = glGenLists(1);
	glNewList(SphereList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(0.f, 2.5f, 0.f);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < sphereObj.numIndices; i++)
	{
		index = sphereObj.indices[i];  //holds the index of the current vertex

		// draw the sphere and the sphere normals
		glNormal3f(sphereObj.normals[index].x, sphereObj.normals[index].y, sphereObj.normals[index].z);
		glVertex3f(sphereObj.vertices[index].x, sphereObj.vertices[index].y, sphereObj.vertices[index].z);
	}
	glEnd();
	glPopMatrix();
	glEndList();

	// the lamp light
	LampLightList = glGenLists(1);
	glNewList(LampLightList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(lampLightPosition.x, lampLightPosition.y, lampLightPosition.z);
	glutSolidSphere(0.15, 20, 20);
	glPopMatrix();
	glEndList();

	//  the lampost object
	LampPostList = glGenLists(1);
	glNewList(LampPostList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(lampostPosition.x, lampostPosition.y, lampostPosition.z);
	glRotatef(135., 0., 1., 0.);
	glScalef(0.1f, 0.1f, 0.1f);
	loadobjReturn = LoadObjFile(fileNameLampost);
	glPopMatrix();
	glEndList();

	// the statue object
	StatueList = glGenLists(1);
	glNewList(StatueList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(statuePosition.x, statuePosition.y, statuePosition.z);
	glRotatef(-60., 0., 1., 0.);
	glRotatef(-90., 1., 0., 0.);
	glScalef(0.01f, 0.01f, 0.01f);
	loadobjReturn = LoadObjFile(fileNameStatue);
	glPopMatrix();
	glEndList();

	// the train object
	TrainList = glGenLists(1);
	glNewList(TrainList, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	glScalef(0.004f, 0.004f, 0.004f);
	loadobjReturn = LoadObjFile(fileNameTrain);
	glPopMatrix();
	glEndList();

	//make the desk object
	DeskList = glGenLists(1);
	glNewList(DeskList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(deskPosition.x, deskPosition.y, deskPosition.z);
	glRotatef(-90., 0., 0., 1.);
	glRotatef(-90., 0., 1., 0.);
	glScalef(0.009f, 0.009f, 0.009f);
	loadobjReturn = LoadObjFile(fileNameDesk);  
	glPopMatrix();
	glEndList();

	//the teddy bear object
	TeddyList = glGenLists(1);
	glNewList(TeddyList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(teddyPosition.x, teddyPosition.y, teddyPosition.z);
	glRotatef(90., 0., 0., 1.);
	glRotatef(90., 0., 1., 0.);
	glScalef(0.02f, 0.02f, 0.02f);
	loadobjReturn = LoadObjFile(fileNameTeddy);  
	glPopMatrix();
	glEndList();

	// teddy bear eyes (red spot light sources)

	// left eye
	TeddyEyeLight1 = glGenLists(1);
	glNewList(TeddyEyeLight1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(teddyEyePosition1.x, teddyEyePosition1.y, teddyEyePosition1.z); 
	glColor3f(1., 0., 0.);
	glutSolidSphere(0.01, 5, 5);
	glPopMatrix();
	glEndList();

	// right eye
	TeddyEyeLight2 = glGenLists(1);
	glNewList(TeddyEyeLight2, GL_COMPILE);
	glPushMatrix();
	glTranslatef(teddyEyePosition2.x, teddyEyePosition2.y, teddyEyePosition2.z);
	glColor3f(1., 0., 0.);
	glutSolidSphere(0.01, 5, 5);
	glPopMatrix();
	glEndList();

	// the porcelain vase object
	VaseList = glGenLists(1);
	glNewList(VaseList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(vasePosition.x, vasePosition.y, vasePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(0.035f, 0.035f, 0.035f);
	loadobjReturn = LoadObjFile(fileNameVase);  
	glPopMatrix();
	glEndList();

	// the shiny vase object
	ShinyVaseList = glGenLists(1);
	glNewList(ShinyVaseList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(shinyVasePosition.x, shinyVasePosition.y, shinyVasePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(0.04f, 0.04f, 0.04f);
	loadobjReturn = LoadObjFile(fileNameShinyVase);  
	glPopMatrix();
	glEndList();

	//make the shiny vase object again for spot light demo
	ShinyVaseSpotLightList = glGenLists(1);
	glNewList(ShinyVaseSpotLightList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(shinyVasePosition.x, shinyVasePosition.y, shinyVasePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(0.1f, 0.1f, 0.1f);
	loadobjReturn = LoadObjFile(fileNameShinyVase);  
	glPopMatrix();
	glEndList();

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( AXES_LENGTH );
		glLineWidth( 1. );
	glEndList( );

	
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		// Freeze or start the animation
		case 'f':
		case 'F':
			//Frozen = !Frozen;
			Frozen = 1 - Frozen;
			if (Frozen) {
				stopTime = glutGet(GLUT_ELAPSED_TIME);
				glutIdleFunc(NULL);
			}	
			else {
				startTime = glutGet(GLUT_ELAPSED_TIME);
				glutIdleFunc(Animate);
			}	
			break;
		// Turn the ambient lighting on/off on the +XZ sector
		case '1':
			//ambientLightPlusXOn = !ambientLightPlusXOn;
			ambientLightPlusXOn = 1 - ambientLightPlusXOn;
			break;
		// Turn the ambient lighting on/off on the -XZ sector
		case '2':
			//ambientLightminusXOn = !ambientLightminusXOn;
			ambientLightminusXOn = 1 - ambientLightminusXOn;
			break;
		// Turn the lamp light on/off
		case 'l':
		case 'L':
			//lampOn = !lampOn;
			lampOn = 1 - lampOn;
			break;	
		// Turn the red spot light on/off
		case 'r':
		case 'R':
			//redSpotOn = !redSpotOn;
			redSpotOn = 1 - redSpotOn;
			break;
		// Turn the train light on/off
		case 't':
		case 'T':
			//trainLightOn = !trainLightOn;
			trainLightOn = 1 - trainLightOn;
			break;

		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not 0x4d42, the file is not a bmp:

	if( FileHeader.bfType != 0x4d42 )
	{
		fprintf( stderr, "File '%s' is the wrong type of file: 0x%0x\n", filename, FileHeader.bfType );
		fclose( fp );
		return NULL;
	}

	FileHeader.bfSize = ReadInt( fp );
	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );
	FileHeader.bfOffBits = ReadInt( fp );

	InfoHeader.biSize = ReadInt( fp );
	InfoHeader.biWidth = ReadInt( fp );
	InfoHeader.biHeight = ReadInt( fp );

	int nums = InfoHeader.biWidth;
	int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	InfoHeader.biBitCount = ReadShort( fp );
	InfoHeader.biCompression = ReadInt( fp );
	InfoHeader.biSizeImage = ReadInt( fp );
	InfoHeader.biXPelsPerMeter = ReadInt( fp );
	InfoHeader.biYPelsPerMeter = ReadInt( fp );
	InfoHeader.biClrUsed = ReadInt( fp );
	InfoHeader.biClrImportant = ReadInt( fp );

	fprintf( stderr, "Image size in file '%s' is: %d x %d\n", filename, nums, numt );

	unsigned char * texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\b" );
		return NULL;
	}

	// extra padding bytes:

	int numextra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;

	// we do not support compression:

	if( InfoHeader.biCompression != birgb )
	{
		fprintf( stderr, "Image file '%s' has the wrong type of image compression: %d\n", filename, InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}

	rewind( fp );
	fseek( fp, 14+40, SEEK_SET );

	if( InfoHeader.biBitCount == 24 )
	{
		unsigned char *tp = texture;
		for( int t = 0; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numextra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	b2 = fgetc( fp );
	b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	unsigned char b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}
