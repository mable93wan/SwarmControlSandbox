#ifndef INCLUDE_POINT_H
#define INCLUDE_POINT_H

#include <iostream>
#include <assert.h>

using namespace std;

class Point2d {
// A 2-d Point.
public:
	double x;
	double y;
	
	Point2d();
	Point2d(double, double);

};

const Point2d& operator+=(Point2d &lhs, const Point2d rhs);
const Point2d operator*(double lhs, const Point2d& rhs);
const Point2d operator+(const Point2d& lhs, const Point2d& rhs);
const Point2d operator-(const Point2d& lhs, const Point2d& rhs);
const Point2d operator-(const Point2d& rhs);
ostream & operator<<(ostream& lhs, const Point2d& rhs);
istream & operator>>(istream& lhs, Point2d& rhs);

bool CCW(Point2d, Point2d, Point2d);
bool CW(Point2d, Point2d, Point2d);
bool colinear(Point2d a, Point2d b, Point2d c);
bool zero(Point2d a);

Point2d perp(Point2d a);
Point2d unit(Point2d a);
double norm(Point2d a);
double dist(Point2d a, Point2d b);
double linfDist(Point2d a, Point2d b);
double squaredDist(Point2d a, Point2d b);
double dot(Point2d a, Point2d b);
Point2d rot(Point2d a, double theta);
bool segmentIntersect(Point2d a, Point2d b, Point2d c, Point2d d);
Point2d lineIntersection(Point2d a, Point2d b, Point2d c, Point2d d);
bool linesHaveIntersection(Point2d a, Point2d b, Point2d c, Point2d d);
bool segmentContains(Point2d a, Point2d b, Point2d c);
bool segmentContainsInInterior(Point2d a, Point2d b, Point2d c);
Point2d transpose(Point2d a);

double pointLineDist(Point2d p, Point2d l1, Point2d l2);
bool pointInSlab(Point2d q, Point2d p1, Point2d p2, Point2d p3, Point2d p4);
bool pointInSlabInterior(Point2d q, Point2d p1, Point2d p2, Point2d p3, Point2d p4);

bool vectorsParallel(Point2d a, Point2d b);
#endif

