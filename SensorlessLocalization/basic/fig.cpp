#include "fig.h"
#include "util.h"
#include <stdio.h>


using namespace std;

Fig::Point::Point(int _x, int _y) : x(_x), y(_y) {

}

string encodeString(string s) {
// Encodes the given string as a sequence of escaped octal ASCII values. 
	string r;
	int i;
	for(i=0;i<int(s.length());i++) {
		if(isalnum(s[i])) {
			r += s[i];
		} else {
			r += formatString("\\%03o", unsigned(s[i]));
		}
	}
	
	return r;

}

Fig::Fig(ostream& _out) : out(_out), sizeInitialized(false), viewInitialized(false) {
	out << "#FIG 3.2" << endl;
	out << "Landscape" << endl;
	out << "Center" << endl;
	out << "Inches" << endl;
	out << "Letter" << endl;
	out << "100.00" << endl; // the magification percent for export and printing
	out << "Single" << endl; // ...as opposed to multiple pages
	out << "-2" << endl; // the color for transparency, -2 disables transparency
	out << FigUnitsPerInch << " ";
	out << "2" << endl; // the coordinate system.  xfig ignores this.  :(

	reset();
}

void Fig::pushSettings() {
	paramStack.push(p);
}


void Fig::popSettings() {
	assert(!paramStack.empty());
	p = paramStack.top();
	paramStack.pop();
}


void Fig::reset() {
	p.depth = 50;
	p.fill = NoFill;
	p.just = Left;
	p.lineStyle = DefaultLineStyle;
	p.dashLength = 1; // TODO
	p.lineThickness = 1;
	p.penColor = DefaultColor;
	p.fillColor = DefaultColor;
	p.joinStyle = Miter;
	p.capStyle = Butt;
	p.forwardArrow = false;
	p.backwardArrow = false;
	points.clear();
}

void Fig::setSize(double _width, double _height) {
	sizeInitialized = true;
	width = _width;
	height = _height;
}

void Fig::setView(double _min_x, double _max_x, double _min_y, double _max_y) {
	viewInitialized = true;
	min_x = _min_x;
	max_x = _max_x;
	min_y = _min_y;
	max_y = _max_y;
}

Fig::Point Fig::mapPoint(double x, double y) {
// Given a point in abstract units, maps it to the coordinates used by the FIG
// format.
	assert(sizeInitialized);
	assert(viewInitialized);

	return Point(
		 int(FigUnitsPerInch * width  * (x-min_x)/(max_x-min_x)),
		-int(FigUnitsPerInch * height * (y-min_y)/(max_y-min_y))
	);

};

double Fig::toVerticalInches(double y) {
// Given a vertical length in abstract units, convert it to a number of inches.
	return y * height / (max_y - min_y);
}

double Fig::toHorizontalInches(double x) {
// Given a horizontal length in abstract units, convert it to a number of
// inches.
	return x * width / (max_x - min_x);
}

void Fig::addText(double x, double y, string text) {
	const int fontSize = 9;
	Point d = mapPoint(x,y);
	if(p.just == Center) {
		// We want (x,y) to be the center of the text, so we need to adjust y --
		// fig expects the coordinate of the bottom.
		d.y += int(((double(fontSize-4))/72.0) * FigUnitsPerInch * 0.5);
	}

	out << Text << " ";
	out << p.just << " ";
	out << p.penColor << " ";
	out << p.depth << " "; // depth
	out << "0 ";  // the "pen style", which is ignored.
	out << DefaultLaTeXFont << " ";
	out << fontSize << " "; // font size in points
	out << "0 "; // angle in radians
	out << "4 "; // font flags 
	out << "1 1 "; // height and width.  I don't think these are important.
	out << d.x << " ";
	out << d.y << " ";
	out << encodeString(text) << "\\001";
	out << endl;

}

void Fig::beginObject() {
	points.clear();
}

void Fig::addPoint(double x, double y) {
	points.push_back(mapPoint(x,y));
}

void Fig::endPolyline() {
	generatePolyline(PolylineST);
}

void Fig::endPolygon() {
	points.push_back(points[0]);
	generatePolyline(Polygon);
	points.erase(points.end()-1);
}

void Fig::generatePolyline(PolylineSubtype type) {
	assert(type != Box);
	assert(type != ImportedPictureBB);
	assert(type != ArcBox);

	out << Polyline << " ";
	out << type << " ";
	out << p.lineStyle << " ";
	out << p.lineThickness << " ";
	out << p.penColor << " ";
	out << p.fillColor << " ";
	out << p.depth << " ";
	out << "0 ";  // the "pen style", which is ignored.
	out << p.fill << " ";
	out << p.dashLength << " ";
	out << p.joinStyle << " ";
	out << p.capStyle << " ";
	out << "0 "; // radius for arc-boxes.  TODO: support this.
	out << p.forwardArrow << " ";
	out << p.backwardArrow << " ";
	out << points.size() << endl;
	
	if(p.forwardArrow)
		out << "0 0 1.00 60.00 120.00" << endl; // forward arrow description TODO
	if(p.backwardArrow)
		out << "0 0 1.00 60.00 120.00" << endl; // backward arrow description TODO
	
	for(int i=0;i<int(points.size());i++)
		out << points[i].x << " " << points[i].y << endl;

}

void Fig::addCircle(double x, double y, double r) {
	generateEllipse(
		EllipseByRadii,
		mapPoint(x,y),
		Point(int(r*FigUnitsPerInch),int(r*FigUnitsPerInch)),
		mapPoint(x,y),
		mapPoint(x+r,y)
	);
}

void Fig::generateEllipse(
	EllipseSubtype type,
	Point center, Point radius,
	Point start, Point end
) {
	out << Ellipse << " ";
	out << type << " ";
	out << p.lineStyle << " ";
	out << p.lineThickness << " ";
	out << p.penColor << " ";
	out << p.fillColor << " ";
	out << p.depth << " ";
	out << "0 ";  // the "pen style", which is ignored.
	out << p.fill << " ";
	out << p.dashLength << " ";
	out << "1 "; // the "direction", also ignored.
	out << "0 "; // the angle of the x-axis. TODO
	out << center.x << " " << center.y << " ";	
	out << radius.x << " " << radius.y << " ";	
	out << start.x << " " << start.y << " ";	
	out << end.x << " " << end.y << endl;
}



// Basically mindless accessor functions.  Someone should add bounds checking
// to these.

void Fig::setDepth(int depth) {
	p.depth = depth;
}
void Fig::increaseDepth() {
	p.depth++;
}
void Fig::decreaseDepth() {
	p.depth--;	
}

int Fig::getDepth() {
	return p.depth;
}

void Fig::setFillPattern(FillPattern fill) {
	p.fill = fill;
}

void Fig::setFillShade(int fill) {
	assert(0 <= fill && fill <= 40);
	p.fill = fill;
}

int Fig::getFill() {
	return p.fill;
}


void Fig::setJustification(Just just) {
	p.just = just;
}
Fig::Just Fig::getJustification() {
	return p.just;
}

void Fig::setLineStyle(LineStyle lineStyle) {
	p.lineStyle = lineStyle;
}
Fig::LineStyle Fig::getLineStyle() {
	return p.lineStyle;
}

void Fig::setDashLength(int dashLength) {
	p.dashLength = dashLength;
}
int Fig::getDashLength() {
	return p.dashLength;
}

void Fig::setLineThickness(int lineThickness) {
	p.lineThickness = lineThickness;
}
int Fig::getLineThickness() {
	return p.lineThickness;
}

void Fig::setColor(Color color) {
	setPenColor(color);
	setFillColor(color);
}
void Fig::nextColor() {
	setColor(Color((p.penColor) % Yellow + 1));
}
void Fig::setPenColor(Color penColor) {
	p.penColor = penColor;
}

Fig::Color Fig::getPenColor() {
	return p.penColor;
}

void Fig::setFillColor(Color fillColor) {
	p.fillColor = fillColor;
}

Fig::Color Fig::getFillColor() {
	return p.fillColor;
}

void Fig::setJoinStyle(JoinStyle joinStyle) {
	p.joinStyle = joinStyle;
}
Fig::JoinStyle Fig::getJoinStyle() {
	return p.joinStyle;
}

void Fig::setCapStyle(CapStyle capStyle) {
	p.capStyle = capStyle;
}
Fig::CapStyle Fig::getCapStyle() {
	return p.capStyle;
}

void Fig::setForwardArrow(bool forwardArrow) {
	p.forwardArrow = forwardArrow;
}

bool Fig::getForwardArrow() {
	return p.forwardArrow;
}

void Fig::setBackwardArrow(bool backwardArrow) {
	p.backwardArrow = backwardArrow;
}

bool Fig::getBackwardArrow() {
	return p.backwardArrow;
}


Fig::Color Fig::encodeColor(double r, double g, double b) {
// Accepts an rgb color specification and returns the appropriate color code.
// This should probably eventually create color pseudo-objects for user-colors,
// but that would require some restructuring, since colors must be defined
// before any real objects in the Fig format.  Hence, these restrictions:
	assert(r==0 || r==1);
	assert(g==0 || g==1);
	assert(b==0 || b==1);
// This function, lamely, also fails to support any of the fancy shades with
// codes greater than 7.
	
	return Color(4*r + 2*g + b) ;

}


void showFig(char const* fileName) {
// This is a utility function, mainly for debugging.
		char buf[100];
		sprintf(buf, "fig2dev -L png -m 2.0 %s /tmp/temp.png && display /tmp/temp.png", fileName);
		system(buf);
}

