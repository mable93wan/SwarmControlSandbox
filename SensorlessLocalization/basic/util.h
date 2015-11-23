/*
 * util.h 
 * 
 * This file has a bunch of definitions for general use, mainly mathish sort of
 * stuff.  There are also a couple of general purpose functions.
 */

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H


#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

#define sign(x) (((x)>0)?1:(((x)==0)?0:-1))
//#define max(a, b) (((a)>(b))?(a):(b))
//#define min(a, b) (((a)<(b))?(a):(b))
#define between(a,b,c) (min((a),(c)) <= (b) && (b) <= max((a),(c)))
#define strictlyBetween(a,b,c) (min((a),(c)) < (b) && (b) < max((a),(c)))

const double PI  = 3.1415926535;
#define deg2rad(x) (PI*((x)/180.0))
#define rad2deg(x) (180.0*((x)/PI))

#define degCos(x) (cos(deg2rad(x)))
#define degSin(x) (sin(deg2rad(x)))
#define degTan(x) (tan(deg2rad(x)))

const double EPSILON = 0.001f;

#ifndef INFINITY
const double INFINITY = 1e10000000;
#endif

const unsigned char ESCAPE = 27;

string formatString(const char *fmt, ...);
void eatComments(ostream& out, istream &in);

int argmin3(double a, double b, double c);

double *canonicalColor(int i);

#endif
