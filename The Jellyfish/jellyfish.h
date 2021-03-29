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

#pragma once
#ifndef JELLYFISH_H
#define JELLYFISH_H

#include "bezierCurve.h"
#include <vector>
#include <stdlib.h> 
#include <time.h>  

using std::vector;

// top position of the jelly fish which is displayed in a system where the axes are 2.5 in length
#define TOP_POSITION 1.f  

// number of curves to make the top which constitutes a stationary small pole
#define NUMCURVES_TOP 10

// maximum radius of the top
#define RADIUSTOP  0.5f

// number of curves to make the middle which is the rest of the main body and can animate
#define NUMCURVES_MIDDLE 20

// maximum radius of the middle
#define RADIUSCENTER 1.f

// number of curves to make the tentacles 
#define NUMCURVES_TENTACLES NUMCURVES_MIDDLE/2


class JellyFish {
private:

	// number of points up to defined parts of the jellyfish
	int numPointsUpToMiddle;
	int numPointsUpToMiddleTrim;
	int numPointsUpToTail;

	// coordinate locations in a cartesian system of defined
	// parts of the jellyfish
	float bottomYofTop;
	float bottomYofMiddle;
	float topYofTentacles;
	float bottomYofTentacles;

	// differential used for distance increments
	float dw;

	// differential used for radius
	float dr;

	// value of the radius at the widest part
	float largestMiddleRadius; 

	// amount to increment angle about the y axis (y is N-S)
	float angleIncr; 

	// holds the curves that make up the jellyfish
	vector<Curve> jellyfish;

	// construct various parts of the jelly fish in the constructor
	void constructTop();
	void constructMiddle();
	void constructTentacles();

	// set x0, y0, z0
	void setOriginalCoordinates();

	// returns a random number between minval and maxval
	float getRand(float minVal, float maxVal);

public:
	JellyFish();  //constructs a jellyfish
	vector<Curve> getJellyFish();  // returns the jellyfish

/*
 * Move the jellyfish using an assortment of standing waves. Send a 
 * standing wave down the entire jellyfish with the top of the 
 * jellyfish being a node and the tip an antinode. Then in addition,
 * at a random wave element to the tentacles so they swing from side
 * to side.
 * The time t is from the system timer used to regulate the timing of
 * of the motion
 */
	bool moveJellyFish(float t);   
};


#endif
