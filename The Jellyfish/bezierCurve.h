/* Utility for constructing curves and points for use Bezier curves 
 *
 *
 *     Description:
 *         struct Point - holds a point in cartesian geometry
 *         struct Curve - holds data for constructing Bezier curves up to order 12
 *         comination(n, r) returns combination(n,r)
 */

#pragma once
#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <stdio.h>

struct Point
{
	float x0, y0, z0; // initial coordinates
	float x, y, z; // animated coordinates
};

struct Curve
{
	float r, g, b; // color of the curve
	Point p[12]; // points on the curve
	long double comb[12];  //holds the coeficient of each term in the curve = combination(n,r)
	float n;  // holds the curve order
};

// returns combination(n,r)
float combination(float n, float r);


#endif