// *****************************************************************
//  This an implementation of my algorithm for our ICRA 05 paper,
//  "Almost-Sensorless Localization".  Given a simple polygon, it computes a
//  sequence of motions to localize a robot in that polygon, given a compass, a
//  contact sensor, and perfect control.
// *****************************************************************

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h> // both for mkdir
#include <sys/types.h>
#include "point.h"
#include "util.h"
#include "fig.h"

using namespace std;

class Polygon {
public:
	vector<Point2d> vertices;

	void draw(Fig &fig) {
	// Draw the polygon onto the figure.
		fig.beginObject();
		for(int i=0;i<int(vertices.size());i++) {
			fig.addPoint(vertices[i].x, vertices[i].y);
		}
		fig.endPolygon();
	}

	Point2d & successor(int v, int s=1) {
	// The s-th successor of vertex v.
		return vertices[(v+s) % vertices.size()];
	}

	double diameter() {
	// This is the largest pairwise distance between any pair of vertices.
		double r = - INFINITY;
		for(int i=0;i<int(vertices.size());i++) {
			for(int j=0;j<int(vertices.size());j++)
				r = max(r, dist(vertices[i], vertices[j]));
		}
		return r;
	}

	double height() {
		double ymin = INFINITY, ymax = -INFINITY;
		for(int j=0;j<int(vertices.size());j++) {
			ymin = min(ymin, vertices[j].y);
			ymax = max(ymax, vertices[j].y);
		}

		return ymax-ymin;
	}

	Point2d centroid() {
	// This is the center point of the polygon.
		Point2d c(0,0);
		for(int i=0;i<int(vertices.size());i++) {
			c += vertices[i];
		}
		return (1.0/vertices.size()) * c;
	}

};

class Environment {
// An environment is just a collection of polygons.  Generally there will be
// one in CCW order representing the outer boundary of the workspace and
// several in CW order representing interior obstacles.

public:
	vector<Polygon> polygons;
	Environment(const char *fileName) {
	// Read in the environment description.  The format is simple:  First, the
	// number of polygons, followed by a description of each one.  A polygon is
	// described by the number of vertices, followed by the coordinates of each
	// vertex.  
	//
	// One special case is when the number of vertices is reported as -1.  The
	// next number is then the real vertex count.  The -1 indicates that the
	// vertex list will appear in reverse order.  This is mainly useful when some
	// stupid software generates the vertex list.
		ifstream in(fileName);


		int numPolygons, numVertices;
		in >> numPolygons;
		cout << "Reading environment with " << numPolygons << " polygons." << endl;
		for(int i=0;i<numPolygons;i++) {
			bool rev = false;
			in >> numVertices;

			if(numVertices == -1) {
				// Special flag to indicate that these vertices will appear in reverse
				// order.
				rev = true;
				in >> numVertices;
			}

			cout << "Reading polygon with " << numVertices << " vertices." << endl;
			Polygon p;
			p.vertices.resize(numVertices);
			for(int j=0;j<numVertices;j++) {
				in >> p.vertices[j];
			}

			if(rev) reverse(p.vertices.begin(), p.vertices.end());
			polygons.push_back(p);

		}
		
		assert(in);
		cout << "...problem loaded!" << endl;
	}


	double diameter() {
	// The largest diameter over any of the polygons in the environment.
		double r = - INFINITY;
		for(int i=0;i<int(polygons.size());i++) {
			r = max(r, polygons[i].diameter());
		}
		return r;
	}

	double height() {
	// The largest height over any of the polygons in the environment.
		double r = - INFINITY;
		for(int i=0;i<int(polygons.size());i++) {
			r = max(r, polygons[i].height());
		}
		return r;
	}
	
	bool segmentFree(Point2d start, Point2d dest) {
	// Returns true if the given segment is fully contained in this environment.

		// I'm not totally sure why this is needed.  TODO
		for(int i=0;i<int(polygons.size());i++)
		for(int j=0;j<int(polygons[i].vertices.size());j++) {
			Point2d p1 = polygons[i].vertices[j];
			Point2d p2 = polygons[i].successor(j);
			
			if(segmentContains(p1, p2, start) && segmentContains(p1, p2, dest))
				return true;

		}

		Point2d impact = shootRay(start, dest-start);
		return !zero(start-impact) && segmentContains(start, impact, dest);
	}

	Point2d shootRay(Point2d start, Point2d direction, int &polygonIndex, int &edgeIndex) {
	// Shoots a ray in the given direction.  This is messier than it seems like
	// it ought to be because I want to allow start points on the boundary and
	// directions along the boundary.
	
	// Most of the work is done in shootRayInternal.  This function only handles
	// the case of shooting along an edge that has a reflex vertex on the other
	// end.  When this happens, we want to keep going, but we get stuck on the
	// other edge of the reflex vertex.  The easiest-to-implement way to avoid
	// this is to keep shooting until we really get stuck.  Since this is the
	// bonehead ray-shooting algorithm anyway, I don't feel bad about this.
		Point2d old, current;
		old = start;
		current = shootRayInternal(start, direction, polygonIndex, edgeIndex);
		while(dist(old, current) > EPSILON) {
			old = current;
			current = shootRayInternal(current, direction, polygonIndex, edgeIndex);
		}

		return current;
	}

	Point2d shootRay(Point2d start, Point2d direction) {
	// Just a special version of the above for when we don't care what edge the
	// impact point is on.
		int dummy;
		return shootRay(start, direction, dummy, dummy);
	}


	Point2d shootRaySweepable(Point2d start, Point2d direction,
		Point2d sweepdir, int &polygonIndex, int &edgeIndex)
	{
	// This is the same as shootRay above, except that it will stop at reflex
	// vertices for which both segments lie on the same side of the shooting ray
	// as the sweepDir.  This is important for the sweep-line portion of the
	// information transition function.
		Point2d old, current;
		old = start;
		current = shootRayInternal(start, direction, polygonIndex, edgeIndex);
		while(dist(old, current) > EPSILON) {
			// Stop if we hit a place that would cause a collision if we moved in the
			// sweep direction.
			Point2d p1 = polygons[polygonIndex].vertices[edgeIndex];
			Point2d p3 = polygons[polygonIndex].successor(edgeIndex, 2);
	
			if(CCW(old, current, current+sweepdir) == CCW(old, current, p1)) break;
			if(CCW(old, current, current+sweepdir) == CCW(old, current, p3)) break;
		
			// Shoot another ray.
			old = current;
			current = shootRayInternal(current, direction, polygonIndex, edgeIndex);
		}

		return current;
	}

	Point2d shootRayInternal(Point2d start, Point2d direction, int &polygonIndex, int &edgeIndex) {
	// Shots a ray from the given starting point in the given direction.  Returns
	// the point intersected.  edgeIndex and polygonIndex are filled in with the
	// number of edge and polygon that generates the collision.  All of of the
	// above versions call this one eventually.

		const bool verbose = false;

		double bestDist = INFINITY;
		Point2d bestPoint;

		// Sanity check.
		assert(!zero(direction));
		
		if(verbose) cout << endl << endl << endl;
		if(verbose) cout << "shooting from " << start << " in direction " << direction << endl;

		// Check for an immediate collision.  There are two cases:  Being in the
		// interior of a segment, and being at its p2 endpoint.
		for(int i=0;i<int(polygons.size());i++) {
			for(int j=0;j<int(polygons[i].vertices.size());j++) {
				Point2d p1 = polygons[i].vertices[j];	
				Point2d p2 = polygons[i].successor(j);
				Point2d p3 = polygons[i].successor(j,2);

				
				// Skip over segments that don't involve the start point.
				if(!segmentContains(p1, p2, start)) continue;

				if(verbose) cout << "  checking for immediate collision with " << p1 << "-" << p2 << endl;
				if(verbose) cout << "  p3 = " << p3 << endl;

				// Skip over segments whose p1 point is the start.   We'll deal with
				// those when the opposite segment comes up.
				if(zero(start-p1)) {
					if (verbose) cout << "    at p1 -- skipping" << endl;
					continue;
				}
				
				// If we get here, one of two things is true: start == p2, or start is
				// in the interior of p1p2.  The later case is easier.
				if(segmentContainsInInterior(p1, p2, start)) {
					if(verbose) cout << "    in interior" << endl;
					if(colinear(p1, p2, start+direction)) {
						if(verbose) cout << "    in interior -- colinear -- no problem" << endl;
						continue;
					}
					if(CCW(p1, p2, start+direction)) {
						if(verbose) cout << "    in interior -- collision -- done" << endl;
						polygonIndex = i;
						edgeIndex = j;
						return start;
					}
					
					if(verbose) cout << "    in interior -- no collision -- no problem" << endl;
					continue;
				}

				// So p2==start.  This test is just a bit different depending on
				// whether or not we're at a reflex vertex.
				if(CW(p1, p2, p3)) {
					// Not a reflex vertex.  An immediate collision results if the motion
					// is into p1p2 OR p2p3.
					if(verbose) cout << "    at p2, not reflex" << endl;
					if(colinear(p1, p2, start+direction) && CW(p2, p3, start+direction)) {
						if(verbose) cout << "    at p2, not reflex, along p1p2, ok" << endl;
						continue;
					}

					if(colinear(p2, p3, start+direction) && CW(p1, p2, start+direction)) {
						if(verbose) cout << "    at p2, not reflex, along p2p3, ok" << endl;
						continue;
					}

					if(CCW(p1, p2, start+direction) || CCW(p2, p3, start+direction)) {
						if(verbose) cout << "    at p2 (nonreflex) -- collision -- done" << endl;
						polygonIndex = i;
						edgeIndex = j;
						return start;
					}
				}

				if(CCW(p1, p2, p3)) {
					// At a reflex vertex.  An immediate collision results if the motion
					// is into p1p2 AND p2p3.
					if(verbose) cout << "    at p2, reflex" << endl;
					if(CCW(p1, p2, start+direction) && CCW(p2, p3, start+direction)) {
						if(verbose) cout << "    at p2 (reflex) -- collision -- done" << endl;
						polygonIndex = i;
						edgeIndex = j;
						return start;
					}
				}

				// If we got here, then we've found a segment containing the start
				// point but not generating an immediate collision.  Assuming the
				// environment isn't too funky (i.e. self-intersecting), we can stop
				// look now.
				if(verbose) cout << "  no immediate collision found." << endl;
				goto done;
			}
		}

		done:
		if(verbose) cout << "  no immediate collision" << endl;

		// By getting here, we've confirmed that there are no immediate collisions.
		// Hence, we can ignore any segment that contains the start point.
		for(int i=0;i<int(polygons.size());i++) {
			for(int j=0;j<int(polygons[i].vertices.size());j++) {
			
				// p1 and p2 are the endpoints of the environment segment we're
				// considering.
				Point2d &p1 = polygons[i].vertices[j];	
				Point2d &p2 = polygons[i].successor(j);

				if(verbose) cout << "  testing against " << p1 << " " << p2 << endl;

				// Skip immediate collision vertices, because we've already dealt with
				// this issue.
				if(segmentContains(p1, p2, start)) continue;

				// Ignore segments parallel to the direction of motion.
				if(!linesHaveIntersection(p1, p2, start, start+direction)) {
					if(verbose) cout << "    parallel to motion direction -- skipping" << endl;
					continue;
				}
				
				// Find out where the ray will hit the line containing p1-p2.
				Point2d intersection = lineIntersection(p1, p2, start, start+direction);
				//if(verbose) cout << "  intersection at " << intersection << endl;
				
				// Does this intersection occur actually within p1-p2?
				if(!segmentContains(p1, p2, intersection)) {
					if(verbose) cout << "    no collision " << intersection << " -- skipping" << endl;
					//if(verbose) cout << "  " << intersection << " is not in " << p1 << "-" << p2 << endl;
					continue;
				}

				// Make sure this intersection point is in the right direction.
				double thisDist = dist(start, intersection);	
				if((direction.x < 0 && (intersection.x - start.x) - EPSILON > 0) 
				|| (direction.x > 0 && (intersection.x - start.x) + EPSILON < 0) 
				|| (direction.y < 0 && (intersection.y - start.y) - EPSILON > 0) 
				|| (direction.y > 0 && (intersection.y - start.y) + EPSILON < 0)) {
					if(verbose) cout << "    intersection is in wrong direction -- skipping" << endl;
					continue;
				}

				if(verbose) cout << "    intersection at distance " << thisDist << endl;

				if(thisDist < bestDist) {
					if(verbose) cout << "    new best" << endl;
					bestDist = thisDist;
					bestPoint = intersection;
					polygonIndex = i;
					edgeIndex = j;
				} else {
					if(verbose) cout << "    keeping current best distance of " << bestDist << endl;
				}
			}
		}
		
		if(!isfinite(bestDist)) {
			// If we get here, it's a bug.
			cout << "BIG PROBLEM!!!! Ray didn't hit anything." << endl;
			cout << "start=" << start << endl;
			cout << "direction=" << direction << endl;
			
			ofstream out("env.fig");
			Fig fig(out);
			fig.setSize(3,3);
			fig.setView(0, diameter(), 0, diameter());
			
			draw(fig);
			
			fig.setColor(Fig::Blue);
			fig.beginObject();
			fig.addPoint(start.x, start.y);
			fig.addPoint(start.x + direction.x, start.y + direction.y);
			fig.endPolyline();
			fig.addCircle(start.x, start.y, 0.02);
			out.close();
			showFig("env.fig");
			
			abort();
		}
		assert(bestDist < INFINITY);

		if(verbose) cout << "  final impact point is " << bestPoint << endl << endl << endl;
		return bestPoint;
	}


	void draw(Fig &fig) {
	// Draws the environment to the given figure.
		for(int i=0;i<int(polygons.size());i++) {
			polygons[i].draw(fig);
		}
	}
};

class Segment {
// A line segment in an information state.
public:
	Point2d p1;
	Point2d p2;

	Segment() {
		// nothing
	}

	Segment(double x1, double y1, double x2, double y2) 
		: p1(x1, y1), p2(x2, y2)
	{
		// nothing
	}

	Segment(Point2d _p1, Point2d _p2) 
		: p1(_p1), p2(_p2)
	{
		// nothing
	}

	void draw(Fig &fig) {
		fig.beginObject();
		fig.addPoint(p1.x, p1.y);
		fig.addPoint(p2.x, p2.y);
		fig.endPolyline();
	}

};

class InfoTransSweepEvent {
// An event for the information transtion sweep line algorithm
public:
	Point2d p;
	int polygon;
	int edge;
};

class EventLineDistCompare {
	Point2d l1, l2;
public:
	EventLineDistCompare(Point2d _l1, Point2d _l2)
		: l1(_l1), l2(_l2) {
		//nothing
	}

	bool operator()(const InfoTransSweepEvent &a, const InfoTransSweepEvent &b) {
		if(fabs(pointLineDist(a.p, l1, l2) - pointLineDist(b.p, l1, l2)) < EPSILON) {
			return dist(a.p, l1) < dist(b.p, l1);
		} else {
			return pointLineDist(a.p, l1, l2) < pointLineDist(b.p, l1, l2);
		}
	}
};

class EventLineDistEqual {
	Point2d l1, l2;
public:
	EventLineDistEqual(Point2d _l1, Point2d _l2)
		: l1(_l1), l2(_l2) {
		//nothing
	}

	bool operator()(const InfoTransSweepEvent &a, const InfoTransSweepEvent &b) {
		return fabs(pointLineDist(a.p, l1, l2) - pointLineDist(b.p, l1, l2)) < EPSILON;
	}
};

class PointOriginDistCompare {
public:
	bool operator()(const Point2d &a, const Point2d &b) {
		return dist(Point2d(0,0), a) < dist(Point2d(0,0), b);
	}
};

class SegmentLeftEndpointCompare {
public:
	bool operator()(const Segment &a, const Segment &b) {
		return a.p1.x < b.p1.x;
	}
};

class InfoState {
// An information state encoding a set of possible positions of the robot.

public:
	vector<Segment> segments;
	vector<Point2d> points;
		// These are the segments that actually comprise the information state.
	
	vector<Segment> segPred;
	vector<Point2d> pointPred;
		// These vectors should have the same length as the previous two.  These
		// vectors list the precessors in of those elements.  This information is
		// needed only for creating animations.

	void initialState(Environment & env) {
	// Generates a new information state that includes the entire boundary of
	// the environment.
		for(int i=0;i<int(env.polygons.size());i++) {
			for(int j=0;j<int(env.polygons[i].vertices.size());j++) {
				points.push_back(env.polygons[i].vertices[j]);
				segments.push_back(Segment(env.polygons[i].vertices[j], env.polygons[i].successor(j)));
			}
		}
	}
	
	void draw(Fig &fig, double pointRadiusInInches = 0.05) {
	// Draw the information state to the given figure using the current settings.
		for(int i=0;i<int(segments.size());i++) {
			segments[i].draw(fig);
		}

		for(int i=0;i<int(points.size());i++) {
			fig.addCircle(points[i].x, points[i].y, pointRadiusInInches);
		}

	}

	double draw(Environment &env, const char *figFile, double sizeInInches=3.0) {
	// Creates a new .fig file with the given name and draws both the environment
	// and this information state onto it.  Return the height of the generated
	// figure in inches.
		ofstream out(figFile);
		Fig fig(out);
		fig.setSize(sizeInInches, sizeInInches);
		fig.setView(0, env.diameter(), 0, env.diameter());
		
		fig.setColor(Fig::Black);
		fig.setLineThickness(2);
		env.draw(fig);
		
		fig.setColor(Fig::Red);
		fig.setFillPattern(Fig::Filled);
		fig.setLineThickness(5);
		fig.decreaseDepth();
		draw(fig, 0.01 * sizeInInches);

		out.close();

		return fig.toVerticalInches(env.height());
	}

	bool sameEdge(Environment &env, Point2d p1, int polygon1, int edge1,
		Point2d p2, int polygon2, int edge2, Point2d &s1, Point2d &s2
	) {
	// Determines whether or not the given two points lie on the same boundary
	// edge.  If so, fills in s1 with these endpoints of the offending edge in
	// CCW order.

		if(polygon1 != polygon2) return false;

		if(segmentContains(env.polygons[polygon1].vertices[edge1],
			env.polygons[polygon1].successor(edge1), p2)
		) {	
			s1 = env.polygons[polygon1].vertices[edge1];
			s2 = env.polygons[polygon1].successor(edge1);
			return true;
		}
						
		if(segmentContains(env.polygons[polygon2].vertices[edge2],
			env.polygons[polygon2].successor(edge2), p1)
		) {
			s1 = env.polygons[polygon2].vertices[edge2];
			s2 = env.polygons[polygon2].successor(edge2);
			return true;
		}
				
		if(abs(edge1 - edge2) == 2) {
			Point2d p3 = env.polygons[polygon1].vertices[(min(edge1, edge2) + 1) % env.polygons[polygon1].vertices.size()];
			Point2d p4 = env.polygons[polygon1].vertices[(min(edge1, edge2) + 2) % env.polygons[polygon1].vertices.size()];

			if((dist(p1, p3) < EPSILON && dist(p2, p4) < EPSILON)
			|| (dist(p1, p4) < EPSILON && dist(p2, p3) < EPSILON))
			{
				s1 = p3;
				s2 = p4;
				return true;
			}
		}
		
		return false;	

	}

	bool sameEdge(Environment &env, Point2d p1, int polygon1, int edge1,
		Point2d p2, int polygon2, int edge2) {
	// Determines whether or not the given two points lie on the same boundary
	// edge.  This version is for those times when we don't care when edge it
	// is.
		Point2d dummy;	
		return sameEdge(env, p1, polygon1, edge1, p2, polygon2, edge2, dummy, dummy);
	}

	void addSegment(Segment s, Segment ps) {
	// Adds a segment without any error checking.  Use the other version instead.
		segments.push_back(s);
		segPred.push_back(ps);
	}


	void addSegment(Environment &env, Point2d p1, int polygon1, int edge1,
			Point2d p2, int polygon2, int edge2, Segment pred
	) {
	// Adds a new segment to the information state given its endpoints.  Tricky
	// because we need to make sure the points are the correct (CCW) order.
		if(!sameEdge(env, p1, polygon1, edge1, p2, polygon2, edge2)) {
			cout << "BIG PROBLEM!  Adding segment not on boundary.\n";
			cout << "  " << p1 << " " << polygon1 << " " << edge1 << endl;
			cout << "  " << p2 << " " << polygon2 << " " << edge2 << endl;

			ofstream out("env.fig");
			Fig fig(out);
			fig.setSize(3,3);
			fig.setView(0, env.diameter(), 0, env.diameter());
			
			env.draw(fig);
			
			fig.setColor(Fig::Green);
			fig.setForwardArrow(true);
			fig.setForwardArrow(false);

			fig.setColor(Fig::Blue);
			fig.beginObject();
			fig.endPolyline();

			fig.setColor(Fig::Red);
			fig.beginObject();
			fig.addPoint(p1.x, p1.y);
			fig.addPoint(p2.x, p2.y);
			fig.endPolyline();

			out.close();
			showFig("env.fig");

			abort();
		}

		Point2d s1, s2;
		sameEdge(env, p1, polygon1, edge1, p2, polygon2, edge2, s1, s2);

		if(between(s1.x, p2.x, p1.x) && between(s1.y, p2.y, p1.y)) {
			addSegment(Segment(p2, p1), pred);
		} else {
			addSegment(Segment(p1, p2), pred);
		}

	}

	void transition(InfoState &result, Environment &env, Point2d direction) {
	// Computes the information state that results from moving in the given
	// direction from this state and puts the result in result.
		const bool verbose = false;

		result.points.clear();
		result.segments.clear();
		result.pointPred.clear();
		result.segPred.clear();

		// The points are easy.
		for(int i=0;i<int(points.size());i++) {
			Point2d	 dest = env.shootRay(points[i], direction);  
			result.points.push_back(dest);
			result.pointPred.push_back(dest);
		}

		// Segments are trickier.
		for(int i=0;i<int(segments.size());i++) {
		
			Point2d & s1 = segments[i].p1;
			Point2d & s2 = segments[i].p2;

			if (verbose) cout << endl << endl << endl;
			if (verbose) cout << "computing resultant of " << segments[i].p1 << "-" << segments[i].p2 << " in direction " << direction << endl;

			if(vectorsParallel(s2 - s1, direction)) {
				if (verbose) cout << "  parallel to motion direction.  just shooting a ray." << endl;
				Point2d x = env.shootRay(s1, direction);
				result.points.push_back(x);
				if(dist(x, s1) < dist(x, s2)) {
					result.pointPred.push_back(s1);
				} else {
					result.pointPred.push_back(s2);
				}
				continue;
			}

			if(CCW(s1, s2, s2 + direction)) {
				if (verbose) cout << "  motion direction into boundary.  no change." << endl;
				result.addSegment(segments[i], segments[i]);
				continue;
			}

			// (1) We'll sweep a line parallel to the direction of motion across the
			// environment. Build a list of all of the vertices of the environment, in
			// the order in which this sweep line will hit them.
			vector<InfoTransSweepEvent> events;
			for(int k=0;k<int(env.polygons.size());k++) {
				for(int j=0;j<int(env.polygons[k].vertices.size());j++) {
					InfoTransSweepEvent e;
					e.p = env.polygons[k].vertices[j];
					e.polygon = k;
					e.edge = j;

					// Eliminate points that are outside the swept segment.
					if(!pointInSlab(e.p, s1, s1+direction, s2, s2+direction)) continue;
					if(dist(e.p, s1) < EPSILON) continue;
					if(dist(e.p, s2) < EPSILON) continue;

					// Eliminate points that the sweep line would hit immediately.  They
					// cause problems.
					if(colinear(e.p, s1, s1+direction)) continue;
					
					// Eliminate points that are occluded.
					int dummy;
					if(!colinear(s1, s2, env.shootRayInternal(e.p, -direction, dummy, dummy))) continue;

					// No objections.  Add the point.
					events.push_back(e);
						
				}
			}

			// A special "termination event".
			if(verbose) cout << "adding termination event" << endl;
			InfoTransSweepEvent e;
			e.p = env.shootRaySweepable(s2, direction, s1-s2, e.polygon, e.edge);
			events.push_back(e);

			// Sort the events into the order in which the sweep line will hit them.
			// Break ties by selecting the event closer to the original segment.
			sort(events.begin(), events.end(), EventLineDistCompare(s1, s1+direction));

			// Eliminate all but the closest in each set of simultaneous events.
			vector<InfoTransSweepEvent>::iterator new_end;
			new_end = unique(events.begin(), events.end(), EventLineDistEqual(s1, s1+direction));
			events.erase(new_end, events.end());

			// (2) Sweep the line.
			Point2d p;
			int pedge, ppolygon;
			p = env.shootRaySweepable(s1, direction, s2-s1, ppolygon, pedge);
			for(int j=0;j<int(events.size());j++) {
				InfoTransSweepEvent &e = events[j];
				if(verbose) cout << "Iteration " << j << endl;
				if(verbose) cout << "  p     is " << p << " " << ppolygon << " " << pedge << endl;
				if(verbose) cout << "  event is " << e.p << " " << e.polygon << " " << e.edge << endl;

				if(sameEdge(env, p, ppolygon, pedge, e.p, e.polygon, e.edge)) {
					// We've reached the end of the current segment.
					if(verbose) cout << "  end of current segment" << endl;
					if(verbose) cout << "  adding segment " << p << "-" << e.p << endl;
					result.addSegment(env, p, ppolygon, pedge, e.p, e.polygon, e.edge, segments[i]);
					p = env.shootRaySweepable(e.p, direction, s2-s1, ppolygon, pedge);
					if(verbose) cout << "  new p is " << p << endl;
				} else {
					// This must be the beginning of a new segment.
					if(verbose) cout << "  start of new segment" << endl;
					int p2polygon, p2edge;
					Point2d p2 = env.shootRay(e.p, direction, p2polygon, p2edge);
					if(verbose) cout << "  adding segment " << p << "-" << p2 << endl;
					result.addSegment(env, p, ppolygon, pedge, p2, p2polygon, p2edge, segments[i]);
					p = e.p;
					ppolygon = e.polygon;
					pedge = e.edge;
					if(verbose) cout << "  new p is " << p << endl;
				}

			}
		}

		// All done!
		result.fixup();

	}

	void fixup() {
	// This function massages the representation of an information state just a
	// bit.  Things should still be technically correct without this, but it
	// makes the display a little nicer.

		// (1) Eliminate zero-length segments.
		for(int i=0;i<int(segments.size());i++) {
			while(i<int(segments.size()) && dist(segments[i].p1, segments[i].p2) < EPSILON) {
				segments.erase(segments.begin() + i);
			}
		}	

		// (2) Remove duplicate points.  This is a little tricky because we want to
		// keep the identities of points 0 and 1 the same -- this is important for
		// the second half of the localization algorithm.  So we remember which 

		// Remeber which points are 0 and 1.
		Point2d p0(points[0]), p1(points[1]);

		// Remove duplicates.
		sort(points.begin(), points.end(), PointOriginDistCompare());
		for(int i=0;i<int(points.size()-1);i++) {
			while(i<int(points.size()-1) && zero(points[i] - points[i+1])) {
				points.erase(points.begin() + i);
			}
		}	

		// Put points 0 and 1 back where they started.
		for(int i=0;i<int(points.size()-1);i++) {
			if(zero(points[i]-p0)) {
				points[i] = points[0];
				points[0] = p0;
			}
			if(zero(points[i]-p1)) {
				points[i] = points[1];
				points[1] = p1;
			}
		}

		// (3) Remove points contained in segments.
		for(int i=0;i<int(segments.size());i++) {
			for(int j=0;j<int(points.size());j++) {
				while(j<int(points.size()) && segmentContainsInInterior(segments[i].p1, segments[i].p2, points[j])) {
					points.erase(points.begin() + j);
				}
			}
		}

	}

};

class VisGraph {
// The visibility graph of an environment, along with functions that use this
// data structure to compute shortest paths in that environment.
protected:
	Environment &env;
	vector<Point2d> nodes;
	vector< vector < bool > > adj;
	
public:
	vector< Point2d > sp;

	VisGraph (Environment & _env) : env(_env) {
	// Initializes the visibility graph for the given environment.

		cout << "Computing visibility graph..." << endl;

		// Start with two nodes for the initial and final states.
		nodes.push_back(Point2d(0,0));
		nodes.push_back(Point2d(0,0));

		// Create a node for each vertex of the environment.
		for(int i=0;i<int(env.polygons.size());i++) {
			for(int j=0;j<int(env.polygons[i].vertices.size());j++) {
				nodes.push_back(env.polygons[i].vertices[j]);
			}
		}

		// Create the adjacency matrix in the correct size.
		adj.resize(nodes.size());
		for(int i=0;i<int(nodes.size());i++) {
			adj[i].resize(nodes.size());
		}

		// Find the adjacent nodes for each vertex.
		for(int i=2;i<int(nodes.size());i++) {
			for(int j=2;j<int(nodes.size());j++) {
				if(i == j)
					adj[i][j] = false;
				else
					adj[i][j] = env.segmentFree(nodes[i], nodes[j]);
			}
		}

		cout << "...done!" << endl;
	}

	double nodeDist(int i, int j) {
	// Returns the (Euclidean) distance between two nodes in the graph.
		return dist(nodes[i], nodes[j]);
	}

	void shortestPath(Point2d start, Point2d dest) {
	// Computes the shortest path from start to dest.  Stores the result in the
	// sp vector.

		// Add the start and dest nodes to the graph.
		nodes[0] = start;
		nodes[1] = dest;
		for(int i=1;i<int(nodes.size());i++) {
			adj[0][i] = zero(nodes[0] - nodes[i]) || env.segmentFree(nodes[0], nodes[i]);
			adj[i][0] = zero(nodes[i] - nodes[0]) || env.segmentFree(nodes[i], nodes[0]);
		}
		for(int i=2;i<int(nodes.size());i++) {
			adj[1][i] = zero(nodes[1] - nodes[i]) || env.segmentFree(nodes[1], nodes[i]);
			adj[i][1] = zero(nodes[i] - nodes[1]) || env.segmentFree(nodes[i], nodes[1]);

		}

		// Bellman-Ford SSSP for shortest paths to dest.
		vector<double> spDist;
		vector<int> spPred;
		spDist.resize(nodes.size());
		spPred.resize(nodes.size());

		for(int i=0;i<int(nodes.size());i++) {
			spDist[i] = INFINITY;
			spPred[i] = -1;
		}
		spDist[1] = 0;

		deque < int > bag;
		bag.push_back(1);

		while(bag.size() > 0) {
			int current = bag[0];
			bag.pop_front();

			// Relax edges leading from current.
			for(int i=0;i<int(nodes.size());i++) {
				if(!adj[current][i]) continue;
				if(spDist[i] > spDist[current] + nodeDist(current, i)) {
					//cout << "relaxing edge from " << current << " to " << i << endl;
					spDist[i] = spDist[current] + nodeDist(current, i);
					spPred[i] = current;
					bag.push_back(i);
				}
			}
		}

		// Extract the path.
		int current = 0;
		sp.clear();
		do {
			sp.push_back(nodes[current]);
			current = spPred[current];
		} while(current != 1);
		sp.push_back(nodes[current]);
		
	}

	void draw(Fig &fig) {
	// Draws the entire visibility graph.
		for(int i=0;i<int(nodes.size());i++) {
			for(int j=0;j<int(nodes.size());j++) {
				if(adj[i][j]) {
					fig.beginObject();
					fig.addPoint(nodes[i].x, nodes[i].y);
					fig.addPoint(nodes[j].x, nodes[j].y);
					fig.endPolyline();
				}
			}
		}
	}

	void drawSP(Fig &fig) {
	// Draws only the most recently-computed shortest path.
		if(sp.size() > 0) {
			fig.beginObject();
			for(int i=0;i<int(sp.size());i++) {
				fig.addPoint(sp[i].x, sp[i].y);
			}
			fig.endPolyline();
		}
	}

	void draw(Environment &env, const char *figFile) {
	// Draws the visiblitity graph and the environment into a new figure with the
	// given name.
		ofstream out(figFile);
		Fig fig(out);
		fig.setSize(3,3);
		fig.setView(0, env.diameter(), 0, env.diameter());
		
		fig.setColor(Fig::Red);
		fig.setFillPattern(Fig::NoFill);
		fig.setLineThickness(3);
		draw(fig);

		fig.decreaseDepth();

		fig.setColor(Fig::Black);
		fig.setFillPattern(Fig::NoFill);
		fig.setLineThickness(3);
		env.draw(fig);
		
		fig.decreaseDepth();
		
		fig.setFillPattern(Fig::NoFill);
		fig.setColor(Fig::Blue);
		drawSP(fig);

		out.close();
	}

};

double drawAction(Point2d u, const char *figFile, double sizeInInches=0.5) {
// Draws an arrow in the given direction to the specified file.  Returns the
// approximate height of the created figure, which will sometimes be needed to
// get things nicely centered in LaTeX.
	ofstream out(figFile);
	Fig fig(out);
	fig.setSize(sizeInInches, sizeInInches);
	fig.setView(-1,1,-1,1);
	
	fig.setColor(Fig::Blue);
	fig.setLineThickness(2);
	fig.setForwardArrow(true);
	fig.beginObject();
	fig.addPoint(unit(-u).x,unit(-u).y);
	fig.addPoint(unit(u).x,unit(u).y);
	fig.endPolyline();

	out.close();
	
	return fig.toVerticalInches(2*fabs(unit(u).y));
}

void fancyInfoStateDraw(Environment &env, InfoState &I, InfoState &I2, Point2d u, const char *fileName) {
// This is mainly for debugging the information transition function.  It draws
// the environment, an action direction, and two information states (intended
// to be before and after).

	ofstream out2(fileName);
	Fig fig(out2);
	fig.setSize(3,3);
	fig.setView(0, env.diameter(), 0, env.diameter());

	env.draw(fig);

	fig.setColor(Fig::Green);
	fig.setLineThickness(4);
	I.draw(fig);
	
	fig.setColor(Fig::Red);
	fig.setLineThickness(2);
	I2.draw(fig);

	fig.setColor(Fig::Blue);
	fig.setLineThickness(2);
	fig.setForwardArrow(true);
	fig.beginObject();
	fig.addPoint(env.polygons[0].centroid().x, env.polygons[0].centroid().y);
	fig.addPoint(env.polygons[0].centroid().x + (0.1 * env.diameter() * unit(u)).x, env.polygons[0].centroid().y + (0.1 * env.diameter() * unit(u)).y);
	fig.endPolyline();

	fig.setColor(Fig::Magenta);
	fig.addCircle(I.points[0].x, I.points[0].y, 0.1);

	fig.setColor(Fig::Cyan);
	fig.addCircle(I.points[1].x, I.points[1].y, 0.1);
	
	out2.close();

}

void generatePlan(Environment &env, vector<Point2d> &plan) {
// This is the main algorithm.

	const bool verbose = false;

	VisGraph vg(env);

	plan.clear();

	InfoState I, I2;
	I.initialState(env);	

	Point2d direction;
	cout << "Generating plan..." << endl;
	
	// (1) Starting with the initial state, reduce the uncertainty to a finite
	// set of possibilities.
	cout << "Stage 1:" << endl;

	while(I.segments.size() > 0) {
		sort(I.segments.begin(), I.segments.end(), SegmentLeftEndpointCompare());
		direction = unit(I.segments[0].p1 - I.segments[0].p2);
		if(direction.x < 0) direction = -direction;

		I.transition(I2, env, direction);
		I = I2;
		plan.push_back(direction);
		cout << direction << endl;
	}

	// (2) Reduce this finite set to a single point.
	cout << "Stage 2:" << endl;
	while(I.points.size() > 1) {
		bool vispair = false;

		for(int i=0;i<int(I.points.size());i++) {
			for(int j=i+1;j<int(I.points.size());j++) {
				if(env.segmentFree(I.points[i], I.points[j])) {
					if(verbose) cout << I.points[0] << " can see " << I.points[1] << endl;
					vispair = true;
					direction = unit(I.points[i] - I.points[j]);
				}	
			}
		}

		if (!vispair) {
			if(verbose) cout << I.points[0] << " is chasing " << I.points[1] << endl;
			vg.shortestPath(I.points[0], I.points[1]);
			direction = unit(vg.sp[1] - I.points[0]);
		}

		I.transition(I2, env, direction);
		plan.push_back(direction);
		cout << direction << endl;

		if(plan.size() > 100 && plan.size() < 110) {
			fancyInfoStateDraw(env, I, I2, direction, "temp.fig");
			showFig("temp.fig");
		}

		I = I2;
	}

	cout << "..." << plan.size() << " step localization sequence computed." << endl;
}

void dumpPlan(Environment &env, vector<Point2d>& plan, const char *dir, double usize, double etasize, int pagesteps) {
// Creates a LaTeX document with associated figures, showing the step-by-step
// progression of information states generated by the given plan.

	cout << "Generateing LaTeX files..." << endl;
	cout << "  usize=" << usize << " " << " etasize=" << etasize << endl;

	mkdir(dir, 0700);
	mkdir(formatString("%s/tablefigs", dir).c_str(), 0700);

	// Open the LaTeX file.
	ofstream out(formatString("%s/table.tex", dir).c_str());

	// Begin a longtable to contain the plan.
	out << "\\begin{longtable}{|c|c|c|}" << endl;
	out << "\\caption{\\aslcaption} \\\\" << endl;
	out << "	\\hline" << endl;
	out << "	$i$ & $u_i$ & $\\eta_{i+1}$ \\\\ \\hline" << endl;
	out << "	\\endfirsthead" << endl;
	out << "" << endl;
	out << "  \\multicolumn{3}{c}{{\\bfseries \\tablename\\ \\thetable{}, cont.}} \\\\" << endl;
	out << "	\\hline" << endl;
	out << "	$i$ & $u_i$ & $\\eta_{i+1}$ \\\\ \\hline" << endl;
	out << "	\\endhead" << endl;
	out << "" << endl;
	out << "	\\multicolumn{3}{|r|}{\\small\\emph{Continued on next page.}} \\\\ \\hline" << endl;
	out << "	\\endfoot" << endl;
	out << "" << endl;
	out << "	\\endlastfoot" << endl;
	out << "" << endl;

	
	out << "" << endl;

	// Generate a figure for each action and information state.
	InfoState I, I2;
	I.initialState(env);	
	double eht = I.draw(env, formatString("%s/tablefigs/%s-eta0000.fig", dir, dir).c_str(), etasize);
	out << "\\aslnewrow" << endl;
	out << "\\raisebox{" << (eht-(2.0/72.0))/2.0 << "in}{0}" << " & ";
	out << " & ";
	out << "\\includegraphics{tablefigs/" << dir << "-eta0000} \\\\ \\hline" << endl << endl;

	for(int i=0;i<int(plan.size());i++) {
		// Compute the next infostate.
		I.transition(I2, env, plan[i]);
		I = I2;
		
		// Draw the new figure.
		cout << "generating figures for step " << i << endl;
		double eht = I.draw(env, formatString("%s/tablefigs/%s-eta%04d.fig", dir, dir, i+1).c_str(), etasize);
		double uht = drawAction(plan[i], formatString("%s/tablefigs/%s-u%04d.fig", dir, dir, i+1).c_str(), usize);

		// Add a line to the LaTeX document to show this new figure.
		if(((i+1)%pagesteps) != 0)
			out << "\\nopagebreak";
		else
			out << "\\pagebreak";
		out << "\\raisebox{" << (eht-(2.0/72.0))/2.0 << "in}{" << (i+1) <<"}" << " & "; // assuming 12pt font
		out << "\\raisebox{" << ((eht-uht)/2) << "in}{\\includegraphics{tablefigs/" << formatString("%s-u%04d", dir, i+1) << "}} ";
		out << " & " << endl;
		out << "\\includegraphics{tablefigs/" << formatString("%s-eta%04d", dir, i+1) << "} \\\\";
		out << "\\hline" << endl;
	}

	// Close the table.
	out << "\\end{longtable}" << endl;
	out.close();

	// Create a wrapper file so that this file can be compiled separately,
	// rather than included in some other document.
	ofstream out2(formatString("%s/wrapper.tex", dir).c_str());
	out2 << "\\documentclass{article}" << endl;
	out2 << "\\usepackage{graphicx}" << endl;
	out2 << "\\usepackage{longtable}" << endl;
	out2 << "\\begin{document}" << endl;
	out2 << "\\newcommand{\\aslcaption}{\\label{tab:1}A " << plan.size() << "-step localizing sequence. \\texttt{" << dir << "}}" << endl;
	out2 << "\\newcommand{\\aslprefix}{.}" << endl;
	out2 << "\\input{table.tex}" << endl;
	out2 << "See Table~\\ref{tab:1}." << endl;
	out2 << "\\end{document}" << endl;
	out2.close();
}

void drawTraces(Environment &env, vector<Point2d>& plan, const char *traceFile, const char *figName) {
// Reads a bunch of starting locations from a file and executes the given plan
// for each one.  The output is a figure with a differently-colored polyline
// for each starting position.

	ifstream in(traceFile);
	
	// Create an empty figure and draw the environment on it.
	ofstream out(figName);
	Fig fig(out);
	fig.setLineThickness(4);
	fig.setSize(6,6);
	fig.setView(0, env.diameter(), 0, env.diameter());
	env.draw(fig);

	// Execute the plan from each starting point listed in the trace file.
	int numTraces;
	in >> numTraces;
	fig.setColor(Fig::Red);
	fig.setLineThickness(2);
	fig.setForwardArrow(true);
	for(int i=0;i<numTraces;i++) {
		Point2d p;
		in >> p;
		cout << "tracing from " << p << endl;
	
		fig.nextColor();
		fig.addCircle(p.x, p.y, 0.01);
		fig.beginObject();
		fig.addPoint(p.x, p.y);
		for(int j=0;j<int(plan.size());j++) {
			p = env.shootRay(p, plan[j]);
			fig.addPoint(p.x, p.y);
			cout << "...to " << p << endl;
		}
		fig.endPolyline();
		fig.addCircle(p.x, p.y, 0.01);

	}

}

void makeInfoStateMovie(Environment & env, vector<Point2d>& plan, const char *dir) {
// Animates the progression of the information state as the plan is executed.
// Writes each frame of the animation into the given directory.

	const double tick = 50;
	int frame = 0;
	InfoState I, I2;

	// Create a figure for the drawing.
	ofstream out("env.fig");
	Fig fig(out);
	fig.setSize(2,2);
	fig.setView(0, env.diameter(), 0, env.diameter());
	env.draw(fig);	

	
	// Treat each step of the plan separately.
	I.initialState(env);
	for(int i=0;i<int(plan.size());i++) {
		// cout << "Animating transition " << i << endl;
		I.transition(I2, env, plan[i]);
		I = I2;

		// Figure out how long each segment in the resulting info state takes to
		// arrive.  Along the way, compute the maximum of these values.
		double maxTime = - INFINITY;
		vector<double> segTime;
		vector<Segment> preds;
		segTime.resize(I2.segments.size());
		preds.resize(I2.segments.size());
		for(int j=0;j<int(I2.segments.size());j++) {
			segTime[j] = pointLineDist(I2.segments[j].p1, I2.segPred[j].p1, I2.segPred[j].p2);
			maxTime = max(maxTime, segTime[j]);
		}
		cout << "  maxTime = " << maxTime << endl;

		if(maxTime == - INFINITY) break;

		// Animate each frame of the current transition.
		double t = 0;
		while (t < maxTime) {
			for(int j=0;j<int(I2.segments.size());j++) {
				if(t >= segTime[j]) {
					fig.setColor(Fig::Red);
					fig.setLineThickness(2);
					I2.segments[j].draw(fig);
				} else {
					double f = (segTime[j] - t);
					Point2d trans = - f * unit(plan[i]);
					Segment s2(I2.segments[j].p1 + trans, I2.segments[j].p2 + trans);
					s2.draw(fig);
					cout << " seg=" << I2.segments[j].p1 << I2.segments[j].p2 << endl;
					cout << " f= " << f << endl;
					cout << " trans= " << trans << endl;
					cout << " s2=" << s2.p1 << s2.p2 << endl;
					cout << endl;
				}
			}
			
			t += tick;
			frame++;
			fig.increaseDepth();
		}

	}

	out.close();
	cout << "Generated " << frame << " frames of output." << endl;

}


int main(int argc, const char **argv) {
	// Load the environment.
	assert(argc >= 2);
	Environment env(argv[1]);

	// Solve the problem.
	vector<Point2d> plan;
	generatePlan(env, plan);
	
	// Process the rest of the command line options, which should be specifying
	// ways to visualize the solution.
	double usize = 0.5;
	double etasize = 1.5;
	int pagesteps = 2;
	for(int i=2;i<argc;i++) {
		// These next two must appear one the command line before "--dump" in order
		// to be effective.
		// --usize: Size of action figures.
		if(!strcmp(argv[i], "--usize")) {
			if(i > argc-1 || 0 == sscanf(argv[i+1], "%lf", &usize))  {
				cerr << argv[0] << ": --usize needs a numerical argument" << endl;
				exit(1);
			}
			i++;
			continue;
		}
		// --etasize: Size of information state figures.
		if(!strcmp(argv[i], "--etasize")) {
			if(i > argc-1 || 0 == sscanf(argv[i+1], "%lf", &etasize))  {
				cerr << argv[0] << ": --etasize needs a numerical argument" << endl;
				exit(1);
			}
			i++;
			continue;
		}
		
		// --pagesteps: Number of steps per page on a dumped plan table.
		if(!strcmp(argv[i], "--pagesteps")) {
			if(i > argc-1 || 0 == sscanf(argv[i+1], "%d", &pagesteps))  {
				cerr << argv[0] << ": --pagesteps needs an integer argument" << endl;
				exit(1);
			}
			i++;
			continue;
		}

		// --dump: dump infostates
		if(!strcmp(argv[i], "--dump")) {
			if(i == argc-1) {
				cerr << argv[0] <<  ": --dump needs a filename argument." << endl;
				exit(0);
			}
			dumpPlan(env, plan, argv[i+1], usize, etasize, pagesteps);
			i++;
			continue;
		}

		if(!strcmp(argv[i], "--trace")) {
			if(i >= argc-2) {
				cerr << argv[0] <<  ": --trace needs two filename arguments." << endl;
				exit(0);
			}
			drawTraces(env, plan, argv[i+1], argv[i+2]);
			i+=2;
			continue;
		}

		if(!strcmp(argv[i], "--movie")) {
			if(i >= argc-1) {
				cerr << argv[0] <<  ": --movie needs a directory argument." << endl;
				exit(0);
			}
			makeInfoStateMovie(env, plan, argv[i+1]);
			i++;
			continue;
		}




		// No options matched.  Bad.
		cerr << argv[0] << ": Unknown option: " << argv[i] << endl;
		exit(1);

	}
	
	return 0;
}

