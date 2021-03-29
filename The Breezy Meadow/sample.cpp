/*
* Description: Added the implementation of a meadow consisting of:
*              - A tree, where the leaves shimmer in the wind
*              - Three species of flowers that move in the wind
*              - Two butterflies that flap their wings and move about the meadow
*              - An apple that falls of the tree, lands on the ground and rolls down the hill
* 
*              To start the wind breezes press the "w" key
*              To drop the apple press the "a" key 
* 
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

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


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glslprogram.h"
#include "loadobjfile.h"



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

const char *WINDOWTITLE = { "The Breezy Meadow" };
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

const GLfloat AXES_WIDTH   = { 3. };
const GLfloat AXES_LENGTH = { 2.6 };

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
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
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

// Use to implement shaders and pass variables to them
GLSLProgram* Pattern;

const float PI = 3.141592654;

// convert degrees to radians:
const float D2R = M_PI / 180.f;

// Timer parameters for wind vibrations
#define MS_IN_THE_ANIMATION_CYCLE 10000 
float currentTime = 0.f;
bool useAnimation = false;
bool setStartValue = false; //set a starting time value
int msStart = 0;

// Timer parameters for apple fall
#define MS_IN_THE_ANIMATION_CYCLE2 10000 
float currentTime2 = 0.f;
bool useAnimation2 = false;
bool setStartValue2 = false; //set a starting time value
int msStart2 = 0;
float finalTime = 0.f;
bool finalTimeSet = false;

// Timer parameters for apple roll
#define MS_IN_THE_ANIMATION_CYCLE3 10000 
float delta = 0.f;
float finalDelta = 0.f;
bool finalDeltaSet = false;
bool useAnimation3 = false;
bool setStartValue3 = true; //set a starting time value
int msStart3 = 0;

// Timer parameters for butterfly flapping wings
#define MS_IN_THE_ANIMATION_CYCLE4 100000 
float currentTime4 = 0.f;
bool useAnimation4 = true;
bool setStartValue4 = true; 
int msStart4 = 0;
bool backward = false;
int msBackwardStart = 0;
int msForwardStart = 0;


// Timer parameters for butterfly zig-zag
#define MS_IN_THE_ANIMATION_CYCLE5 100000 
float currentTime5 = 0.f;
bool useAnimation5 = true;
bool setStartValue5 = true; 
int msStart5 = 0;

// Timer parameters for butterfly steady
#define MS_IN_THE_ANIMATION_CYCLE6 100000 // 
float currentTime6 = 0.f;
bool useAnimation6 = false;
bool setStartValue6 = true; 
int msStart6 = 0;

#define GRAVITY 19.6  //  double value of gravity to compensate for slower timer

bool appleFall = true;  // flag to indicate apple falling from tree is true if select apple fall
bool appleRoll = false; // this flag becomes true after the apple has landed on the ground and is ready to roll downhill

// Positioning of objects
glm::vec3 grassBoundary = glm::vec3(-1.3f, -0.5f, -2.0f);
glm::vec3 treePosition = glm::vec3(grassBoundary.x, grassBoundary.y + 0.4f, 0.0f);
glm::vec3 applePosition = glm::vec3(treePosition.x + 0.5f, 1.3f, 0.0f); 

// display lists for the indicated object
GLuint	grassList;				
GLuint	treeTrunkList;				
GLuint	treeFruitList;				
GLuint	treeLeavesList;				
GLuint	appleList;				
GLuint	butterflyList;			
GLuint	butterflyList2;				
GLuint	daisyList;				
GLuint	whiteFlowerList;
GLuint	snowdropList;
GLuint	woodList;

// Textures for the indicated object
unsigned char* Texture;
GLuint	grassTex;
GLuint	barkTex;
GLuint	leafTex;
GLuint	appleTex;
GLuint	appleWholeTex;
GLuint	butterflyTex;
GLuint	butterflyTex2;
GLuint	daisyTex;
GLuint	whiteFlowerTex;
GLuint	snowdropTex;
GLuint	woodTex;
GLuint	uTexUnit;


// main program:

int
main( int argc, char *argv[ ] )
{
	// seed the random number generator
	srand(time(NULL));

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

	int ms = 0; // milliseconds  
	int ms2 = 0; // milliseconds  
	int ms3 = 0; // milliseconds  
	int ms4 = 0; // milliseconds 
	int ms5 = 0; // milliseconds 
	int ms6 = 0; // milliseconds 
	float epsilon = 0.01;  // use for implementing floating type == where instead use 
	                       // param > value - epsilon && param < value + epsilon
	float timeSpan2 = 4.0f;  // how fast to drop the apple
	float minYheight = 0.32f*timeSpan2;  // indicates when apple has reached the ground
	float maxLinearDistance = 0.77f; // maximum distance apple advances
	float timerRateWind = 6.f;  //use to control timer rate for wind
	float timerRateWingFlap = 100.f;
	float timerRateZigZag = 10000.f;
	float timerRateflowersOsc = 100.f;
	float maxWindTime = 7.0f;  // maximum time to let pass before resetting the wind timer
	float randomTime;
	
	randomTime = (float)rand() / RAND_MAX;  // random number between 0 and 1

	maxWindTime = maxWindTime - 3.f * randomTime;  //max time before wind timer gets reset to zero indicating a new blast of wind

	if (useAnimation) {  //use for vibrations and motion damping in the wind
		ms = glutGet(GLUT_ELAPSED_TIME);
		if (setStartValue) {
			msStart = ms;  // when timer restarts at zero there is a new blast of wind
			setStartValue = false;
		}	
		ms = (ms - msStart); 
		currentTime = (float)ms*timerRateWind / ((float)MS_IN_THE_ANIMATION_CYCLE); // starts at 0 when reset

		if (currentTime > maxWindTime - epsilon)
		{
			setStartValue = true;
			setStartValue6 = true;
		}
	}

	if (useAnimation2) {   // use for falling apple falling at the rate of gravity to the ground
		ms2 = glutGet(GLUT_ELAPSED_TIME);
		if (setStartValue2) {  // only used if reset is selected
			msStart2 = ms2;  
			setStartValue2 = false;
		}
		if (finalTimeSet) {   // start the next timer which is used for the apple to roll down the hill after hitting the ground
			currentTime2 = finalTime;
			useAnimation3 = true;  
		}
		else 
		{
			ms2 = (ms2 - msStart2) % MS_IN_THE_ANIMATION_CYCLE2;
			currentTime2 = (float)ms2*timeSpan2 / ((float)MS_IN_THE_ANIMATION_CYCLE2); // starts at 0 when reset
		}
		if (finalTimeSet == false && 0.5 * GRAVITY * currentTime2 * currentTime2 > (minYheight - epsilon)) // when apple has reached the ground
		{
			finalTimeSet = true;
			finalTime = currentTime2;
			appleFall = false;
			appleRoll = true;
			useAnimation3 = true;  // start the next timer which is used for the apple to roll down the hill after hitting the ground
		}
		
	}
	if (useAnimation3) {  //use for rolling apple, doubles over as dx and dt so call this delta instead of currentTime
		ms3 = glutGet(GLUT_ELAPSED_TIME);
		
		if (setStartValue3) {	// only used if reset is selected
			msStart3 = ms3;  
			setStartValue3 = false;
		}
		if (finalDeltaSet)
			delta = finalDelta;
		else
		{
			ms3 = (ms3 - msStart3);
			delta = (float)ms3 / ((float)MS_IN_THE_ANIMATION_CYCLE3); // starts at 0 when reset
		}
		if (finalDeltaSet == false && delta > (maxLinearDistance - epsilon))  // if have reached maximum distance then stop the timer
		{
			finalDeltaSet = true;
			finalDelta = delta;
			appleFall = false; 
		}			
	}

	if (useAnimation4) {  //use for butterfly wings flapping
		ms4 = glutGet(GLUT_ELAPSED_TIME);

		if (setStartValue4) {
			msStart4 = ms4;  
			setStartValue4 = false;
		}
		ms4 = (ms4 - msStart4)% MS_IN_THE_ANIMATION_CYCLE4;
		currentTime4 = (float)ms4 / timerRateWingFlap;   // divide the time by a factor that results in a reasonable flat rate
		currentTime4 = sinf(currentTime4);  // the timer ranges from -1 to +1
	}

	if (useAnimation5) {  //use for butterfly zigzag across meadow
		ms5 = glutGet(GLUT_ELAPSED_TIME);

		if (setStartValue5) {
			msStart5 = ms5;  
			setStartValue5 = false;
		}
		ms5 = (ms5 - msStart5) % MS_IN_THE_ANIMATION_CYCLE5;
		currentTime5 = (float)ms5 / timerRateZigZag; // divide the time by a factor that results in a reasonable travel rate
		currentTime5 = sinf(currentTime5);	// the timer ranges from -1 to +1
	}

	if (useAnimation6) {  //use for flowers osciallating in the wind
		ms6 = glutGet(GLUT_ELAPSED_TIME);
		if (setStartValue6) {
			msStart6 = ms6;  
			setStartValue6 = false;
		}
		ms6 = (ms6 - msStart6)% MS_IN_THE_ANIMATION_CYCLE6;
		currentTime6 = (float)ms6 / timerRateflowersOsc;	// divide the time by a factor that results in a reasonable oscillation rate
		currentTime6 = fabs(sinf(currentTime6)); // the timer ranges from 0 to +1 then +1 down to 0 and so on
	}

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	glm::vec3 objectColor;  // color of the object
	int objectId[] = { 0,1,2,3,4,5,6,7,8,9 }; // the object id - used by the vertex shader to know which motion to use
	float flowerDamp; // rate at which flower oscillations in wind damp
	float oscRate;  // rate at which flower oscillates in the wind
	float omegaf; // how much the flower head moves by in radians
	// wind comes in from left side so flowers closer to the left oscillate first
	float xRange = 4; // width of grass patch to flowers from -x to x
	float tdelay;  //time by which to delay flowers oscillating in the wind according to their location

	// --- Flower positions around the meadow ---
	// flowers are strategically placed rather than randomly since the flower population and meadow size is small

	// around the flat part of the meadow
	glm::vec3 daisyPosition = glm::vec3(0.8f, grassBoundary.y + 0.15f, 0.f);
	glm::vec3 whiteFlowerPosition = glm::vec3(0.6f, grassBoundary.y + 0.15f, 0.2f);
	glm::vec3 snowdropPosition = glm::vec3(1.0f, grassBoundary.y + 0.15f, 0.5f);
	glm::vec3 daisyPosition2 = glm::vec3(0.2f, grassBoundary.y + 0.15f, -0.5f);
	glm::vec3 snowdropPosition2 = glm::vec3(0.5f, grassBoundary.y + 0.15f, -0.7f);
	glm::vec3 daisyPosition3 = glm::vec3(0.9f, grassBoundary.y + 0.15f, 1.5f);
	glm::vec3 whiteFlowerPosition3 = glm::vec3(0.f, grassBoundary.y + 0.15f, -1.5f);
	glm::vec3 whiteFlowerPosition4 = glm::vec3(2.f, grassBoundary.y + 0.15f, -1.3f);
	glm::vec3 snowdropPosition5 = glm::vec3(0.f, grassBoundary.y + 0.15f, 1.f);
	glm::vec3 snowdropPosition6 = glm::vec3(1.7f, grassBoundary.y + 0.15f, 0.8f);

	//on the hill
	glm::vec3 whiteFlowerPosition2 = glm::vec3(-0.2f, grassBoundary.y + 0.25f, 1.f);
	glm::vec3 daisyPosition6 = glm::vec3(-0.5f, grassBoundary.y + 0.25f, treePosition.z + -1.f);
	glm::vec3 daisyPosition7 = glm::vec3(treePosition.x + 0.4f, grassBoundary.y + 0.25f, treePosition.z);
	glm::vec3 daisyPosition8 = glm::vec3(-1.f, grassBoundary.y + 0.4f, treePosition.z + 1.4f);

	// around the tree
	glm::vec3 daisyPosition4 = glm::vec3(treePosition.x - 0.3f, grassBoundary.y + 0.38f, treePosition.z + 0.4);
	glm::vec3 daisyPosition5 = glm::vec3(treePosition.x - 0.1f, grassBoundary.y + 0.4f, treePosition.z + 0.5);
	glm::vec3 snowdropPosition3 = glm::vec3(treePosition.x - 0.2f, grassBoundary.y + 0.39f, -0.2f);
	glm::vec3 snowdropPosition4 = glm::vec3(treePosition.x - 0.1f, grassBoundary.y + 0.39f, -0.3f);
	

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


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


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
	glLoadIdentity();

	glm::mat4 projection;
	if (WhichProjection == ORTHO)
		projection = glm::ortho(-3., 3., -3., 3., 0.1, 1000.);
	else
		projection = glm::perspective(D2R * 90., 1., 0.1, 1000.);
	
	// apply the projection matrix:
	glMultMatrixf(glm::value_ptr(projection));

	// place the objects into the scene:
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	glm::vec3 eye(0., 0., AXES_LENGTH + 0.5);
	glm::vec3 look(0., 0., 0.);
	glm::vec3 up(0., 1., 0.);
	glm::mat4 modelview = glm::lookAt(eye, look, up);

	// rotate the scene:
	modelview = glm::rotate(modelview, D2R * Yrot, glm::vec3(0., 1., 0.));
	modelview = glm::rotate(modelview, D2R * Xrot, glm::vec3(1., 0., 0.));
	
	
	// uniformly scale the scene:
	if( Scale < MINSCALE )
		Scale = MINSCALE;
	modelview = glm::scale(modelview, glm::vec3(Scale, Scale, Scale));
	
	
	// apply the modelview matrix:
	glMultMatrixf(glm::value_ptr(modelview));
	

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

	glEnable( GL_NORMALIZE );

	glShadeModel(GL_SMOOTH); 


	// insert objects here ---------------

	
	// draw objects via vertex and fragment shaders
	Pattern->Use();

	// pass values to the shaders
	Pattern->SetUniformVariable("animation1", useAnimation);
	Pattern->SetUniformVariable("appleMotion", useAnimation2);
	Pattern->SetUniformVariable("t6", currentTime6);  


	// grass meadow
	objectColor = glm::vec3(1.f, 1.f, 1.f);
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[0]);

	glActiveTexture(GL_TEXTURE1); // use texture unit 1
	glBindTexture(GL_TEXTURE_2D, grassTex);
	Pattern->SetUniformVariable("uTexUnit", 1); 
	glCallList(grassList);
	

	//tree trunk and branches
	objectColor = glm::vec3(0.447f, 0.361f, 0.259f);
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[1]);

	glActiveTexture(GL_TEXTURE2); // use texture unit 2
	glBindTexture(GL_TEXTURE_2D, barkTex);
	Pattern->SetUniformVariable("uTexUnit", 2); 

	glCallList(treeTrunkList);
	
	
	// tree leaves
	Pattern->SetUniformVariable("t1", currentTime);
	objectColor = glm::vec3(0.075f, 0.306f, 0.075f);
	
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[2]);

	glActiveTexture(GL_TEXTURE3); // use texture unit 3
	glBindTexture(GL_TEXTURE_2D, leafTex);
	Pattern->SetUniformVariable("uTexUnit", 3);

	glCallList(treeLeavesList);
	

	// tree fruit
	objectColor = glm::vec3(1.f, 1.f, 1.f);  
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[3]);

	glActiveTexture(GL_TEXTURE4); // use texture unit 4
	glBindTexture(GL_TEXTURE_2D, appleTex);
	Pattern->SetUniformVariable("uTexUnit", 4);

	glCallList(treeFruitList);

	// apple
	objectColor = glm::vec3(1.f, 1.f, 1.f);
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[4]);

	Pattern->SetUniformVariable("appleFall", appleFall);
	Pattern->SetUniformVariable("appleRoll", appleRoll);
	Pattern->SetUniformVariable("t2", currentTime2);
	Pattern->SetUniformVariable("delta", delta);

	glActiveTexture(GL_TEXTURE5); // use texture unit 5
	glBindTexture(GL_TEXTURE_2D, appleWholeTex);
	Pattern->SetUniformVariable("uTexUnit", 5);

	glCallList(appleList);


	// Butterfly  
	objectColor = glm::vec3(1.f, 0.984f, 0.773f);  
	
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[5]);
	Pattern->SetUniformVariable("t4", currentTime4); 
	Pattern->SetUniformVariable("t5", currentTime5);  //for zigzag motion
	
	glActiveTexture(GL_TEXTURE6); // use texture unit 6
	glBindTexture(GL_TEXTURE_2D, butterflyTex);
	Pattern->SetUniformVariable("uTexUnit", 6);

	glCallList(butterflyList);

	// second butterfly 
	Pattern->SetUniformVariable("objectId", objectId[9]);
	glActiveTexture(GL_TEXTURE7); // use texture unit 7
	glBindTexture(GL_TEXTURE_2D, butterflyTex2);
	Pattern->SetUniformVariable("uTexUnit", 7);

	glCallList(butterflyList2);

	// daisies 
	objectColor = glm::vec3(0.79687f, 0.79687f, 0.99609); 
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[6]);

	// how quickly the oscillatory motion damps
	flowerDamp = 0.4f; 
	Pattern->SetUniformVariable("flowerDamp", flowerDamp);

	// how fast the flower moves back and forth
	oscRate = 0.3f; 
	Pattern->SetUniformVariable("oscRate", oscRate);

	// how much the flower moves back and forth
	omegaf = 1.0f;
	Pattern->SetUniformVariable("omegaf", omegaf);

	tdelay = (xRange/2.f - daisyPosition.x) / xRange;
	Pattern->SetUniformVariable("tdelay", tdelay);


	glActiveTexture(GL_TEXTURE8); // use texture unit 8
	glBindTexture(GL_TEXTURE_2D, daisyTex);
	Pattern->SetUniformVariable("uTexUnit", 8);

	glPushMatrix();
	glTranslatef(daisyPosition.x, daisyPosition.y, daisyPosition.z);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition2.x, daisyPosition2.y, daisyPosition2.z);
	glRotatef(90., 0., 1., 0.);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition3.x, daisyPosition3.y, daisyPosition3.z);
	glRotatef(30., 0., 1., 0.);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition4.x, daisyPosition4.y, daisyPosition4.z);
	glRotatef(-90., 0., 1., 0.);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition5.x, daisyPosition5.y, daisyPosition5.z);
	glRotatef(60., 0., 1., 0.);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition7.x, daisyPosition7.y, daisyPosition7.z);
	glCallList(daisyList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(daisyPosition8.x, daisyPosition8.y, daisyPosition8.z);
	glCallList(daisyList);  
	glPopMatrix();

	// whiteflowers
	objectColor = glm::vec3(0.79687f, 0.79687f, 0.99609); 
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[7]);

	flowerDamp = 0.8f; 
	Pattern->SetUniformVariable("flowerDamp", flowerDamp);

	oscRate = 0.3f;
	Pattern->SetUniformVariable("oscRate", oscRate);

	// how much the flower moves back and forth
	omegaf = 0.5f;
	Pattern->SetUniformVariable("omegaf", omegaf);

	tdelay = (xRange / 2.f - whiteFlowerPosition.x) / xRange;
	Pattern->SetUniformVariable("tdelay", tdelay);

	glActiveTexture(GL_TEXTURE9); // use texture unit 9
	glBindTexture(GL_TEXTURE_2D, whiteFlowerTex);
	Pattern->SetUniformVariable("uTexUnit", 9);

	glPushMatrix();
	glTranslatef(whiteFlowerPosition.x, whiteFlowerPosition.y, whiteFlowerPosition.z);
	glCallList(whiteFlowerList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(whiteFlowerPosition2.x, whiteFlowerPosition2.y, whiteFlowerPosition2.z);
	glRotatef(-70., 0., 1., 0.);
	glCallList(whiteFlowerList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(whiteFlowerPosition3.x, whiteFlowerPosition3.y, whiteFlowerPosition3.z);
	glRotatef(-90., 0., 1., 0.);
	glCallList(whiteFlowerList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(whiteFlowerPosition4.x, whiteFlowerPosition4.y, whiteFlowerPosition4.z);
	glRotatef(60., 0., 1., 0.);
	glCallList(whiteFlowerList);  
	glPopMatrix();

	// snowdrop flowers
	objectColor = glm::vec3(0.773f, 0.788f, 1.f);
	Pattern->SetUniformVariable("objectColor", objectColor);
	Pattern->SetUniformVariable("objectId", objectId[8]);

	flowerDamp = 0.65f;
	Pattern->SetUniformVariable("flowerDamp", flowerDamp);

	oscRate = 0.4f;
	Pattern->SetUniformVariable("oscRate", oscRate);

	// how much the flower moves back and forth
	omegaf = 0.75f;
	Pattern->SetUniformVariable("omegaf", omegaf);

	tdelay = (xRange / 2.f - snowdropPosition.x) / xRange;
	Pattern->SetUniformVariable("tdelay", tdelay);

	glActiveTexture(GL_TEXTURE10); // use texture unit 10
	glBindTexture(GL_TEXTURE_2D, snowdropTex);
	Pattern->SetUniformVariable("uTexUnit", 10);

	glPushMatrix();
	glTranslatef(snowdropPosition.x, snowdropPosition.y, snowdropPosition.z);
	glCallList(snowdropList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(snowdropPosition2.x, snowdropPosition2.y, snowdropPosition2.z);
	glRotatef(90., 0., 1., 0.);
	glCallList(snowdropList); 
	glPopMatrix();

	glPushMatrix();
	glTranslatef(snowdropPosition3.x, snowdropPosition3.y, snowdropPosition3.z);
	glRotatef(-70., 0., 1., 0.);
	glCallList(snowdropList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(snowdropPosition4.x, snowdropPosition4.y, snowdropPosition4.z);
	glRotatef(30., 0., 1., 0.);
	glCallList(snowdropList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(snowdropPosition5.x, snowdropPosition5.y, snowdropPosition5.z);
	glRotatef(60., 0., 1., 0.);
	glCallList(snowdropList);  
	glPopMatrix();

	glPushMatrix();
	glTranslatef(snowdropPosition6.x, snowdropPosition6.y, snowdropPosition6.z);
	glCallList(snowdropList);  
	glPopMatrix();
	
	Pattern->Use(0);

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
	/*
	glDisable( GL_DEPTH_TEST );
	glColor3f( 0., 1., 1. );
	DoRasterString( 0., 1., 0., "Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., "Text That Doesn't" );
	*/

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
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
	glutIdleFunc( Animate);

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

	// do this *after* opening the window and init'ing glew:

	// setup the vertex and fragment shaders
	Pattern = new GLSLProgram();
	bool validP = Pattern->Create("pattern.vert", "pattern.frag");

	if (!validP)
	{
		fprintf(stderr, "Shader cannot be created!\n");
		DoMainMenu(QUIT);
	}
	else
	{
		fprintf(stderr, "Shader created.\n");
	}
	Pattern->SetVerbose(false);

	// ----- Set up textures -------

	// grass texture
	glGenTextures(1, &grassTex);
	int width = 1024, height = 1024;
	Texture = BmpToTexture("textures/grassPatch.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// bark texture
	glGenTextures(1, &barkTex);
	width = 512; 
	height = 1024;
	Texture = BmpToTexture("textures/bark.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, barkTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// leaf texture
	glGenTextures(1, &leafTex);
	width = 256; 
	height = 256;
	Texture = BmpToTexture("textures/leaf.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, leafTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// apple texture for fruit on tree
	glGenTextures(1, &appleTex);
	width = 512;
	height = 256;
	Texture = BmpToTexture("textures/apple.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, appleTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// apple texture for whole apple
	glGenTextures(1, &appleWholeTex);
	width = 1024;
	height = 1024;
	Texture = BmpToTexture("textures/appleWhole.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, appleWholeTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// yellow butterfly texture
	glGenTextures(1, &butterflyTex);
	width = 256;
	height = 256;
	Texture = BmpToTexture("textures/yellowButterfly.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, butterflyTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// daisy texture
	glGenTextures(1, &daisyTex);
	width = 1024;
	height = 1024;
	Texture = BmpToTexture("textures/daisy.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, daisyTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// white flower texture
	glGenTextures(1, &whiteFlowerTex);
	width = 1024;
	height = 1024;
	Texture = BmpToTexture("textures/whiteFlower.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, whiteFlowerTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// snowdrop texture
	glGenTextures(1, &snowdropTex);
	width = 1024;
	height = 1024;
	Texture = BmpToTexture("textures/snowdrop.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, snowdropTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// orange butterfly texture
	glGenTextures(1, &butterflyTex2);
	width = 256;
	height = 256;
	Texture = BmpToTexture("textures/orangeButterfly.bmp", &width, &height);
	glBindTexture(GL_TEXTURE_2D, butterflyTex2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	// get and load the object files
	int loadobjReturn;
	char* fileNameGrass = "objects/grass.obj"; 
	char* fileNameTreeTrunk = "objects/treeTrunk.obj";
	char* fileNameTreeLeaves = "objects/treeLeaves.obj";
	char* fileNameTreeFruit = "objects/treeFruit.obj";
	char* fileNameApple = "objects/apple.obj";
	char* fileNameButterfly = "objects/butterfly.obj";
	char* fileNameDaisy = "objects/daisy.obj";
	char* fileNameWhiteFlower = "objects/whiteFlower.obj";
	char* fileNameSnowdrop = "objects/snowdrop.obj";
	char* fileNameWood = "objects/wood.obj";
	
	// Positions of various objects
	glm::vec3 grassScale = glm::vec3(0.01667f, 0.0133, 0.0133f); 
	glm::vec3 butterflyPosition = glm::vec3(1.0f, grassBoundary.y + 01.f, 0.2f);
	glm::vec3 butterflyPosition2 = glm::vec3(0.4f, grassBoundary.y + 0.8f, -0.8f);
	glm::vec3 woodPosition = glm::vec3(1.3f, grassBoundary.y - 0.2f, -1.5f);
	
	// Scaling factor to use with various objects based on their original sizes
	float treeScale = 0.03f;
	float leavesScale = 0.03f;
	float fruitScale = 0.03f;
	float appleScale = 2.0f; 
	float butterflyScale = 0.02;
	float daisyScale = 0.04;
	float whiteFlowerScale = 0.02;
	float snowdropScale = 0.07;
	float woodScale = 0.07;

	glutSetWindow( MainWindow );

	// -----create the objects-----:

	// create the grass object
	grassList = glGenLists(1);
	glNewList( grassList, GL_COMPILE );
	glPushMatrix();
	glTranslatef(0.f, grassBoundary.y, 0.f);
	glRotatef(-90., 1., 0., 0.);
	glScalef(grassScale.x, grassScale.y, grassScale.z);
	loadobjReturn = LoadObjFile(fileNameGrass);
	glPopMatrix();
	glEndList( );
	
	// create the tree trunk/branches object
	treeTrunkList = glGenLists(1);
	glNewList(treeTrunkList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(treePosition.x, treePosition.y, treePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(treeScale, treeScale, treeScale);
	loadobjReturn = LoadObjFile(fileNameTreeTrunk);
	glPopMatrix();
	glEndList();
	
	// create the tree fruit object
	treeFruitList = glGenLists(1);
	glNewList(treeFruitList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(treePosition.x, treePosition.y, treePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(fruitScale, fruitScale, fruitScale);
	loadobjReturn = LoadObjFile(fileNameTreeFruit);
	glPopMatrix();
	glEndList();

	// create the tree leaves object
	treeLeavesList = glGenLists(1);
	glNewList(treeLeavesList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(treePosition.x, treePosition.y, treePosition.z);
	glRotatef(-90., 1., 0., 0.);
	glScalef(leavesScale, leavesScale, leavesScale);
	loadobjReturn = LoadObjFile(fileNameTreeLeaves);
	glPopMatrix();
	glEndList();

	// create the whole apple object
	appleList = glGenLists(1);
	glNewList(appleList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(applePosition.x, applePosition.y, applePosition.z);
	glScalef(appleScale, appleScale, appleScale);
	loadobjReturn = LoadObjFile(fileNameApple);
	glPopMatrix();
	glEndList();

	// create the yellow butterfly object
	butterflyList = glGenLists(1);
	glNewList(butterflyList, GL_COMPILE);
	glPushMatrix();
	glTranslatef(butterflyPosition.x, butterflyPosition.y, butterflyPosition.z);
	glRotatef(270., 0., 1., 0.);
	glScalef(butterflyScale, butterflyScale, butterflyScale);
	loadobjReturn = LoadObjFile(fileNameButterfly);
	glPopMatrix();
	glEndList();

	// create the orange butterfly object
	butterflyList2 = glGenLists(1);
	glNewList(butterflyList2, GL_COMPILE);
	glPushMatrix();
	glTranslatef(butterflyPosition2.x, butterflyPosition2.y, butterflyPosition2.z);
	glRotatef(180., 0., 1., 0.);
	glScalef(butterflyScale, butterflyScale, butterflyScale);
	loadobjReturn = LoadObjFile(fileNameButterfly);
	glPopMatrix();
	glEndList();

	// create the daisy object
	daisyList = glGenLists(1);
	glNewList(daisyList, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	glScalef(daisyScale, daisyScale, daisyScale);
	loadobjReturn = LoadObjFile(fileNameDaisy);
	glPopMatrix();
	glEndList();

	// create the white flower object
	whiteFlowerList = glGenLists(1);
	glNewList(whiteFlowerList, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	glScalef(whiteFlowerScale, whiteFlowerScale, whiteFlowerScale);
	loadobjReturn = LoadObjFile(fileNameWhiteFlower);
	glPopMatrix();
	glEndList();

	// create the snowdrop object
	snowdropList = glGenLists(1);
	glNewList(snowdropList, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0.);
	glScalef(snowdropScale, snowdropScale, snowdropScale);
	loadobjReturn = LoadObjFile(fileNameSnowdrop);
	glPopMatrix();
	glEndList();
	

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
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
		case 'a':
		case 'A':
			useAnimation2 = true;
			setStartValue2 = true; //set a starting time value for apple dropping
			useAnimation3 = false;
			setStartValue3 = true; //set a starting time value for apple dropping
			finalTimeSet = false;
			finalDeltaSet = false;
			appleFall = true;
			appleRoll = false;
			//finalTime = 0.f;
			//currentTime2 = 0.f;
			break;
		case 's':
		case 'S':
			useAnimation2 = false;
			useAnimation3 = false;
			break;
		case 'w':
		case 'W':
			useAnimation = true;
			setStartValue = true; //set a starting time value for wind blowing on tree
			useAnimation6 = true;
			setStartValue6 = true; //set a starting time value for wind blowing on flowers
			break;
		case 'b':
		case 'B':
			useAnimation4 = true;
			setStartValue4 = true; //butterfly moving
			useAnimation5 = true;
			setStartValue5 = true; //butterfly moving
			break;
		case 'f':
		case 'F':
			useAnimation = false;
			useAnimation4 = false;
			useAnimation5 = false;
			useAnimation6 = false;
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
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;

	currentTime = 0.f;
	useAnimation = false;
	setStartValue = true; //set a starting time value
	msStart = 0;

	currentTime2 = 0.f;
	useAnimation2 = false;
	setStartValue2 = true; //set a starting time value
	finalTime = 0.f;
	msStart2 = 0;
	finalTimeSet = false;
	appleFall = true;
	appleRoll = false;

	delta = 0.f;
	useAnimation3 = false;
	setStartValue3 = true;
	msStart3 = 0;

	currentTime4 = 0.f;
	useAnimation4 = true;
	setStartValue4 = true; //set a starting time value
	msStart4 = 0;

	currentTime5 = 0.f;
	useAnimation5 = true;
	setStartValue5 = true; //might not need
	msStart5 = 0;

	currentTime6 = 0.f;
	useAnimation6 = false;
	setStartValue6 = true; //might not need
	msStart6 = 0;
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
