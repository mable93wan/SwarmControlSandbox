#include <math.h>
#include "util.h"
#include "point.h"


Point2d::Point2d() : x(0), y(0) {

}


Point2d::Point2d(double x_in, double y_in) : x(x_in), y(y_in) {

}


const Point2d& operator+=(Point2d &lhs, const Point2d rhs) {
	lhs = lhs + rhs;
	return lhs;
}

const Point2d operator*(double lhs, const Point2d& rhs) {
	return Point2d(lhs * rhs.x, lhs * rhs.y);
}

const Point2d operator+(const Point2d& lhs, const Point2d& rhs) {
	return Point2d(lhs.x + rhs.x, lhs.y + rhs.y);
}

const Point2d operator-(const Point2d& lhs, const Point2d& rhs) {
	return Point2d(lhs.x - rhs.x, lhs.y - rhs.y);
}
const Point2d operator-(const Point2d& rhs) {
	return Point2d(-rhs.x, -rhs.y);
}

ostream & operator<<(ostream& lhs, const Point2d& rhs) {
	lhs << "(" << rhs.x << "," << rhs.y << ")";
	return lhs;
}

istream & operator>>(istream& lhs, Point2d& rhs) {
	lhs >> rhs.x >> rhs.y;
	return lhs;
}


bool CCW(Point2d a, Point2d b, Point2d c) {
	return (c.y-a.y)*(b.x-a.x) > (b.y-a.y)*(c.x-a.x);
}

bool CW(Point2d a, Point2d b, Point2d c) {
	return (c.y-a.y)*(b.x-a.x) < (b.y-a.y)*(c.x-a.x);
}

bool colinear(Point2d a, Point2d b, Point2d c) {
	//cout << "colinear:" << a << b << c << 
	//	" " << fabs((c.y-a.y)*(b.x-a.x) - (b.y-a.y)*(c.x-a.x)) << endl;

	return fabs((c.y-a.y)*(b.x-a.x) - (b.y-a.y)*(c.x-a.x)) < EPSILON;
}


bool zero(Point2d a) {
	return fabs(a.x) < EPSILON && fabs(a.y) < EPSILON;
}

Point2d perp(Point2d a) {
	return Point2d(-a.y, a.x);
}

Point2d unit(Point2d a) {
	return (1.0/norm(a)) * a;
}

double norm(Point2d a) {
	return sqrt(a.x*a.x + a.y*a.y);
}

double dist(Point2d a, Point2d b) {
	return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

double linfDist(Point2d a, Point2d b) {
	return max(fabs(a.x - b.x), fabs(a.y - b.y));
}

double squaredDist(Point2d a, Point2d b) {
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}


double dot(Point2d a, Point2d b) {
	return a.x * b.x + a.y * b.y;
}

Point2d rot(Point2d a, double theta) {
	double s = degSin(theta);
	double c = degCos(theta);
	return Point2d(dot(a, Point2d(c, -s)), dot(a, Point2d(s, c)));
}

bool segmentIntersect(Point2d a, Point2d b, Point2d c, Point2d d) {
	// What the heck.
	/*
	if(dist(a,c) < EPSILON || dist(b,d) < EPSILON) return true;
	if(dist(a,d) < EPSILON || dist(b,c) < EPSILON) return true;
	*/

	return (CCW(a,c,d) != CCW(b,c,d)) && (CCW(a,b,c) != CCW(a,b,d));
}
	
bool linesHaveIntersection(Point2d a, Point2d b, Point2d c, Point2d d) {
// Does the line between a and b intersect the line between c and d?

	double m1 = (a.y - b.y) / (a.x - b.x);
	double m2 = (c.y - d.y) / (c.x - d.x);
	if(!isfinite(m1) && !isfinite(m2)) return false;
	return (fabs(m1-m2) > EPSILON);
}

Point2d lineIntersection(Point2d a, Point2d b, Point2d c, Point2d d) {
// Computes the Point2d of intersection of two lines.  Gets grumpy when called on
// non-intersecting lines.

	bool v1 = fabs(a.x-b.x) < EPSILON;
	bool v2 = fabs(c.x-d.x) < EPSILON;
	if(!v1 && !v2) {
		// Neiter segment is vertical.
		double m1 = (a.y - b.y) / (a.x - b.x);
		double m2 = (c.y - d.y) / (c.x - d.x);
		assert(fabs(m1-m2) > EPSILON);
		double x = (m1*a.x - m2*c.x + c.y - a.y)/(m1-m2);
		return Point2d(x, m1*(x-a.x)+a.y);
	} else if(v1) {
		// a-b is vertical.
		double m2 = (c.y - d.y) / (c.x - d.x);
		return Point2d(a.x, m2*(a.x-c.x)+c.y);
	} else if(v2) {
		// c-d is vertical.
		double m1 = (a.y - b.y) / (a.x - b.x);
		return Point2d(c.x, m1*(c.x-a.x)+a.y);
	} else {
		// Both vertical.  Ick.
		assert(false);
	}
}

bool segmentContainsInInterior(Point2d a, Point2d b, Point2d c) {

	return (segmentContains(a,b,c) && !zero(a-c) && !zero(b-c));

	/*
	if(!colinear(a,b,c))
		return false;

	if(fabs(a.x-b.x) > EPSILON) {
		return strictlyBetween(a.x, c.x, b.x);
	} else {
		return strictlyBetween(a.y, c.y, b.y);
	}
	*/

}

bool segmentContains(Point2d a, Point2d b, Point2d c) {
// Does the segment between a and b contain Point2d c?
	if(min(dist(a,c),dist(b,c)) < EPSILON)
		return true;
	
	if(!colinear(a,b,c))
		return false;

	if(fabs(a.x-b.x) > EPSILON) {
		return between(a.x, c.x, b.x);
	} else {
		return between(a.y, c.y, b.y);
	}
}

Point2d transpose(Point2d a) {
	return Point2d(a.y, a.x);
}


double pointLineDist(Point2d p, Point2d l1, Point2d l2) {
	static Point2d proj;

	if(fabs(l2.x - l1.x) < EPSILON) {
		// Vertical line.  Special case.
		return fabs(p.x - l1.x);
	} else {
		proj = lineIntersection(l1, l2, p, p+perp(l2-l1));
		return dist(proj, p);	
	}
}

bool pointInSlab(Point2d q, Point2d p1, Point2d p2, Point2d p3, Point2d p4) {
	assert(!linesHaveIntersection(p1,p2,p3,p4));
	return 
		colinear(p1, p2, q)
		|| colinear(p3, p4, q)
		|| CW(p1, p2, q) != CW(p3, p4, q);
}

bool pointInInterior(Point2d q, Point2d p1, Point2d p2, Point2d p3, Point2d p4) {
	assert(!linesHaveIntersection(p1,p2,p3,p4));
	if(colinear(p1, p2, q)) return false;
	if(colinear(p3, p4, q)) return false;
	return CW(p1, p2, q) != CW(p3, p4, q);
}

bool vectorsParallel(Point2d a, Point2d b) {
	return fabs(dot(a, perp(b))) < EPSILON;
}
