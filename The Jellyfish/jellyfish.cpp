/*
*
* Description: Instantiation returns a jellyfish constructed from Bezier curves. Key functions include:
* 
*       JellyFish();  //construct a jellyfish
*	    vector<Curve> getJellyFish();  // returns the jellyfish points
*
*	    bool moveJellyFish(float t);   //moves the jellyfish points   
*       The time t is from the system timer used to regulate the timing of of the motion.
*           
*       The jellyfish coordinates range from about +1 at the top to -1
*
*       The coordinated axis used in the construction has the y-axis oriented to the North.
*/

#include "jellyfish.h"

/* constructor constructs the jellyfish by default. The jellyfish is contructed by creating idential curves that vary 
 * according to the shape in (x,z) and placing these symmetrically around the y axis analogous to lines of longitude
 * around a sphere.
*/
JellyFish::JellyFish()
{	
	dw = 0.1;
	srand(time(NULL));
	constructTop();
	constructMiddle();
	constructTentacles();
	setOriginalCoordinates();
}

// returns the jellyfish
vector<Curve> JellyFish::getJellyFish() {
	return jellyfish;
}

// set the original coordinates x0, y0, z0 before any motion takes place
void JellyFish::setOriginalCoordinates()
{
	int numVertices = jellyfish.size();
	int n;

	for (int i = 0; i < numVertices; i++) {
		n = jellyfish[i].n;
		for (int j = 0; j < n + 1; j++) {
			jellyfish[i].p[j].x0 = jellyfish[i].p[j].x;
			jellyfish[i].p[j].y0 = jellyfish[i].p[j].y;
			jellyfish[i].p[j].z0 = jellyfish[i].p[j].z;
		}
	}
}

// returns a random number between minval and maxval
float JellyFish::getRand(float minVal, float maxVal) {
	return minVal + (maxVal - minVal) * rand() / RAND_MAX;
}

/*
 * Move the jellyfish using an assortment of standing waves. Send a 
 * standing wave down the entire jellyfish with the top of the 
 * jellyfish being a node and the tip an antinode. Then in addition,
 * at a random wave element to the tentacles so they swing from side
 * to side.
 * The time t is from the system timer used to regulate the timing of
 * of the motion
 */
bool JellyFish::moveJellyFish(float t) { 
	int n; //curve order
	int j; 
	int numVerticesSoFar = jellyfish.size();
	float amp= dw/2.f; //amplitude of the motion
	float signX,signZ;  // sign of the vertex point coordinates
	bool timeReset = false;  // reset the external timer
	// The wave length to use in standing wave motion is twice the length of the jellyfish
	float lambda =  2.f*(bottomYofTop - bottomYofTentacles);
	// Standing wave parameter
	float k =  2.0* M_PI / lambda;
	// wave frequency
	float omega =  2.0 *M_PI * 10.0;

	for (int i = NUMCURVES_TOP; i < numVerticesSoFar; i++) {
		n = jellyfish[i].n;
		for (int j = 0; j < n + 1; j++) {

			if (jellyfish[i].p[j].x < 0.f)
				signX = -1.0;
			else
				signX = 1.0;
			if (jellyfish[i].p[j].z < 0.f)
				signZ = -1.0;
			else
				signZ = 1.0;

			float sMiddle = bottomYofTop - jellyfish[i].p[j].y;
			float sTentacles = topYofTentacles - jellyfish[i].p[j].y;

			// implement a standing wave to mimic jellyfish swimming
			jellyfish[i].p[j].y = jellyfish[i].p[j].y0 +  2.0 * amp * cos(omega * t) * sin(k * sMiddle);
			
			if (i < numPointsUpToTail) {
				jellyfish[i].p[j].x = jellyfish[i].p[j].x0 + amp * signX * cos(omega * t) * sin(k * sMiddle);
				jellyfish[i].p[j].z = jellyfish[i].p[j].z0 + amp * signZ * cos(omega * t) * sin(k * sMiddle);
			}
			
			else if (i >= numPointsUpToTail) {
				jellyfish[i].p[j].x = jellyfish[i].p[j].x0 + amp * signX * cos(omega * t) * sin(k * sMiddle/10.f);
				jellyfish[i].p[j].z = jellyfish[i].p[j].z0 + amp * signZ * cos(omega * t) * sin(k * sMiddle/10.f);
			}
		
			// add an additional wave so tail swings from side to side too
			if (i  >= numPointsUpToMiddleTrim && i < numPointsUpToTail)
			{
				jellyfish[i].p[j].x =  jellyfish[i].p[j].x +getRand(4.f*amp, 5.f*amp) * cos(omega * t) *sin(M_PI/4.f * sTentacles/ (topYofTentacles - bottomYofTentacles));
				jellyfish[i].p[j].z = jellyfish[i].p[j].z +  getRand(4.f*amp,  5.f*amp)* cos(omega * t) * sin(M_PI / 4.f * sTentacles/(topYofTentacles-bottomYofTentacles));
			}		
		}
	}

		return timeReset;
}

/* Construct the tentacles from Bezier curves. There are two sets of tentacles, outer
 * and inner. The outer tentacles start from the jelly fish middle, while the innter
 * tentacles originate from the center of the jellyfish and are concentric within the
 * outer ones. Each tentacle is described by the same curves arranged symmetrically 
 * around the North-South axis.
 */
void JellyFish::constructTentacles() {  
	int numVerticesSoFar = jellyfish.size();
	int numVerticesUpToMiddle = numVerticesSoFar;
	int numPointsTentacles = 10;  // number of points that make up each curve in the tentacles
	float r = 1.f, g = 0.82f, b = 0.863f;  // color of the tentacles
	float height = bottomYofMiddle;  // starting height on the coordinated display axis of the tentacles
	float dheight = dw;  // height differential between points going down the tentacles
	float finalRadius = dw; // Final radius value of outer tentacles which taper in
	float startRadiusDiff = dw; // starting radius value for the inner tentacles
	float dradius = (largestMiddleRadius / 2.f - 0.f) / (2 * numPointsTentacles -1); // radius differential between points going down the curve
	float radiusInnerTaildiff = 0.95f*largestMiddleRadius;  // radius location of the inner tentacles
	float signX, signZ;
	int n;

	int lastMiddlePoint = 0;
	int lastTentaclePoint = numPointsTentacles;
	
	Curve temp;

	//Both the outer/inner tentacles are made from two consecutive curves to get the
	//desired length since each curve is limited to 10 points.

	// define a set of curves of the same shape and place them around a circle such that
	// the jellyfish is symetrial about the y-axis

	// Outer Tail first set of curves since curves are limited to 11 points
	for (int i = numVerticesSoFar, j = numVerticesSoFar - NUMCURVES_MIDDLE, w=0; i < numVerticesSoFar + NUMCURVES_TENTACLES; i++, j+=2, w+=2) {
		jellyfish.push_back(temp);
		
		n = jellyfish[i].n = numPointsTentacles;  // order of the Bezier curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		

		for (int k = 0; k < n + 1; k++) //move so tentacle top sits inside of curved Middle
		{
			if (jellyfish[i].p[k].x < 0.f)
				signX = -1.0;
			else
				signX = 1.0;
			if (jellyfish[i].p[k].z < 0.f)
				signZ = -1.0;
			else
				signZ = 1.0;

			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r
			// radius varies from k*dradius where k = 0 to n+1
			// hieght varies from k*dheight 

			jellyfish[i].p[k].x = jellyfish[j].p[lastMiddlePoint].x -startRadiusDiff* cosf(angleIncr * w) - k*dradius * cosf(angleIncr * w);
			jellyfish[i].p[k].y = jellyfish[j].p[lastMiddlePoint].y - k*dheight;
			jellyfish[i].p[k].z = jellyfish[j].p[lastMiddlePoint].z - startRadiusDiff*sinf(angleIncr * w) - k*dradius * sinf(angleIncr * w);
			jellyfish[i].comb[k] = combination(n, k);
		}
		topYofTentacles = jellyfish[i].p[0].y;
	}

	numVerticesSoFar = jellyfish.size();

	// outer Tail second set of curves since curves are limited to 11 points
	for (int i = numVerticesSoFar, j = numVerticesSoFar - NUMCURVES_TENTACLES, w=0; i < numVerticesSoFar + NUMCURVES_TENTACLES; i++, j++, w+=2) {
		jellyfish.push_back(temp);

		n = jellyfish[i].n = numPointsTentacles;	// order of the Bezier curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		for (int k = 0; k < n + 1; k++) //move so tentacle top sits inside of curved Middle
		{
			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r
			// radius varies from k*dradius where k = 0 to n+1
			// hieght varies from k*dheight 

			jellyfish[i].p[k].x = jellyfish[j].p[lastTentaclePoint].x - k * dradius * cosf(angleIncr * w);
			jellyfish[i].p[k].y = jellyfish[j].p[lastTentaclePoint].y - k * dheight;
			jellyfish[i].p[k].z = jellyfish[j].p[lastTentaclePoint].z - k * dradius * sinf(angleIncr * w);
			jellyfish[i].comb[k] = combination(n, k);
		}
		bottomYofTentacles = jellyfish[i].p[numPointsTentacles].y;
	}

	numVerticesSoFar = jellyfish.size();
	numPointsUpToTail = numVerticesSoFar;

	r = 0.941f; g = 0.455f; b = 0.318f;   // color of the inner tentacles

	// Inner Tail first set of curves since curves are limited to 11 points
	for (int i = numVerticesSoFar, j = numVerticesUpToMiddle - NUMCURVES_MIDDLE, w = 0; i < numVerticesSoFar + NUMCURVES_TENTACLES; i++, j += 2, w += 2) {
		jellyfish.push_back(temp);

		n = jellyfish[i].n = numPointsTentacles;	// order of the Bezier curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		for (int k = 0; k < n + 1; k++) 
		{
			if (jellyfish[i].p[k].x < 0.f)
				signX = -1.0;
			else
				signX = 1.0;
			if (jellyfish[i].p[k].z < 0.f)
				signZ = -1.0;
			else
				signZ = 1.0;

			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r
			// radius varies from k*dradius where k = 0 to n+1
			// hieght varies from k*dheight 

			jellyfish[i].p[k].x = jellyfish[j].p[lastMiddlePoint].x - radiusInnerTaildiff * cosf(angleIncr * w);
			jellyfish[i].p[k].y = jellyfish[j].p[lastMiddlePoint].y - k * dheight;
			jellyfish[i].p[k].z = jellyfish[j].p[lastMiddlePoint].z - radiusInnerTaildiff * sinf(angleIncr * w);
			jellyfish[i].comb[k] = combination(n, k);
		}

	}

	numVerticesSoFar = jellyfish.size();

	// Inner Tail second set of curves since curves are limited to 11 points
	for (int i = numVerticesSoFar, j = numVerticesSoFar - NUMCURVES_TENTACLES, w = 0; i < numVerticesSoFar + NUMCURVES_TENTACLES; i++, j++, w += 2) {
		jellyfish.push_back(temp);

		n = jellyfish[i].n = numPointsTentacles - 2;	// order of the Bezier curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		for (int k = 0; k < n + 1; k++) //move so tentacle top sits inside of curved Middle
		{
			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r
			// hieght varies from k*dheight 

			jellyfish[i].p[k].x = jellyfish[j].p[lastTentaclePoint].x;
			jellyfish[i].p[k].y = jellyfish[j].p[lastTentaclePoint].y - k * dheight;
			jellyfish[i].p[k].z = jellyfish[j].p[lastTentaclePoint].z;
			jellyfish[i].comb[k] = combination(n, k);
		}
	}
}

/* Construct the middle of the jellyfish from Bezier curves. The middle
 * and the top together constitute the bulbous body of the jelly fish. 
 * However, in the current construction the middle section can move while
 * the top part remains stationary. Note that the jellyfish is symmetrical
 * about the y axis.
 */
void JellyFish::constructMiddle() {
	int numVerticesSoFar = jellyfish.size();
	int numVerticesStartMiddle = numVerticesSoFar;
	float numPointsToEquator = 4;   // number of points in the curves to the widest part of the body
	float radius = 2.f* RADIUSTOP / 3.f;  // starting radius
	float height = bottomYofTop;  // starting height
	float radiusInit = radius; // starting radius
	float heightInit = height; // starting height
	float dheight = dw/1.5f ; // differential between points going down a curve
	float dradius = 0.5f*(RADIUSCENTER - RADIUSTOP)/(numPointsToEquator);  // differential between points radially
	
	float mid_angle;
	float r = 1.f, g = 0.82f, b = 0.863f;   // color of the middle 
	angleIncr = 2.0 * M_PI / NUMCURVES_MIDDLE;  // differential of the angle between curves as they placed symmetrically about y
	int j,l,n;
	Curve temp;

	// define a set of curves of the same shape and place them around a circle such that
	// the jellyfish is symmetrial about the y-axis
	for (int i = numVerticesSoFar, w=0; i < numVerticesSoFar + NUMCURVES_MIDDLE; i++, w++) //build from the top down
	{
		jellyfish.push_back(temp);

		n = jellyfish[i].n = 10;   // order of the curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		radius = radiusInit;
		height = heightInit;

		for (int k = 0; k < n + 1; k++) {

			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r

			jellyfish[i].p[k].x = radius * cosf(angleIncr * w);
			jellyfish[i].p[k].y = height;
			jellyfish[i].p[k].z = radius * sinf(angleIncr * w);
			jellyfish[i].comb[k] = combination(n, k);

			if (k < 5) {
				radius +=  dradius;    // increase the radius with successive points along a curve until radius = largestMiddleRadius
			}
			if (k > 8)
			{
				radius -=  dradius;  // the radius goes in towards the end of the curve to give an overall lightbulb shape
			}

			height -= dheight;   // decrease the height in y with successive points along the curve
		}

		largestMiddleRadius = radius;
	}

	bottomYofMiddle = height;

	//Create a trim around the jellyfish torso permimeter. The trim is made up of consecutive curves of order two
	//that string together around a certain hieght of the main body. Each curve links up two neighbouring longitudinal
	//points of two neighbouring curves laid symmetrically about the y axis. In otherwords the trim can be thought up
	//as a single latitude line connecting neighbouring longitude lines about a sphere.

	numVerticesSoFar = jellyfish.size();

	// color of the trim
	r = 1.f;
	g = 0.549f;
	b = 0.412f;

	numPointsUpToMiddle = numVerticesSoFar;

	for (int i = numVerticesSoFar, k = 0; i < numVerticesSoFar + NUMCURVES_MIDDLE; i++, k++)  //check upper limit
	{
		j = numVerticesStartMiddle + k;
		l = numVerticesStartMiddle + (k+1)%NUMCURVES_MIDDLE;

		jellyfish.push_back(temp);

		jellyfish[i].n = 2;  // order of the curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		jellyfish[i].p[0].x = jellyfish[j].p[5].x;
		jellyfish[i].p[0].y = jellyfish[j].p[5].y;
		jellyfish[i].p[0].z = jellyfish[j].p[5].z;
		jellyfish[i].comb[0] = combination(2, 0);

		
		jellyfish[i].p[1].x = jellyfish[j].p[4].x + (jellyfish[l].p[4].x - jellyfish[j].p[4].x)/2.f;
		jellyfish[i].p[1].y = jellyfish[j].p[4].y;
		jellyfish[i].p[1].z = jellyfish[j].p[4].z + (jellyfish[l].p[4].z - jellyfish[j].p[4].z)/2.f;
		jellyfish[i].comb[1] = combination(2, 1);

		jellyfish[i].p[2].x = jellyfish[l].p[5].x;
		jellyfish[i].p[2].y = jellyfish[l].p[5].y;
		jellyfish[i].p[2].z = jellyfish[l].p[5].z;
		jellyfish[i].comb[2] = combination(2, 2);

	}

	numPointsUpToMiddleTrim = jellyfish.size();
}

/* Construct the top, which is a small stationary pole area. The top is constructed by creating identical curves
 * and placing them symmetrically about the y-axis such that a spherical pole region is created.
 */
void JellyFish::constructTop() {
	int numPointsInCurve = 3, n;  // the order of each curve
	float dradius = RADIUSTOP / 4.f;  // radius differental between successive points going down a curve.
	float radiusInit = 0.f, heightInit = TOP_POSITION;  // starting values of the radius and height
	float radius = 0.f, height;
	float dheight = dw / 4.f;  // height differental between successive points going down a curve.
	float angleIncr = 2.0*M_PI / NUMCURVES_TOP;  // angle differential between neighbouring curves
	int numVerticesSoFar;
	float r = 0.941f, g = 0.455f, b = 0.318f; // top color
	
	dr = RADIUSTOP / 6.f;
	Curve temp;

	// define a set of curves of the same shape and place them around a circle such that
	// the jellyfish is symetrial about the y-axis

	for (int i = 0; i < NUMCURVES_TOP; i++) 
	{
		jellyfish.push_back(temp);

		n = jellyfish[i].n = numPointsInCurve;  // order of the curves

		jellyfish[i].r = r;
		jellyfish[i].g = g;
		jellyfish[i].b = b;

		radius = radiusInit;
		height = heightInit;

		for (int k = 0; k < n + 1; k++) {

			//use the fact that x = radius*cos(angle) and z = radius*sin(angle) places the point (x,z) on a circle with radius r

			jellyfish[i].p[k].x = radius * cosf(angleIncr * i);
			jellyfish[i].p[k].y = height;
			jellyfish[i].p[k].z = radius * sinf(angleIncr * i);
			jellyfish[i].comb[k] = combination(n, k);

			radius += dradius;	// increase the radius with successive points along a curve
			height -= dheight;	// decrease the height in y with successive points along the curve
		}


	}

	bottomYofTop = height + dheight;

	// Create a trim around the bottom of the top pole. This curve can be thought of as a single latitude connecting
	// nieghbouring longitudes on a sphere

	for (int j = 1; j < numPointsInCurve + 1; j++)
	{
		numVerticesSoFar = jellyfish.size();

		for (int i = numVerticesSoFar, l = 0; i < numVerticesSoFar + numPointsInCurve; i++, l++)  //check upper limit
		{
		
			jellyfish.push_back(temp);

			n = jellyfish[i].n = NUMCURVES_TOP;  // order of the curve

			jellyfish[i].r = r;
			jellyfish[i].g = g;
			jellyfish[i].b = b;

			for (int k = 0; k < n + 1; k++) {
				l = k % NUMCURVES_TOP;
				jellyfish[i].p[k].x = jellyfish[l].p[j].x;
				jellyfish[i].p[k].y = jellyfish[l].p[j].y;
				jellyfish[i].p[k].z = jellyfish[l].p[j].z;
				jellyfish[i].comb[k] = combination(n, k);
			}

		}
	}


}