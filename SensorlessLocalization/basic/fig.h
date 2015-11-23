#ifndef INCLUDE_FIG_H
#define INCLUDE_FIG_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

class Fig {
public:
	enum ObjectType {
		Ellipse  = 1,
		Polyline = 2,
		Spline   = 3,
		Text     = 4,
		Arc      = 5,
		Compound = 6
	};

	enum PolylineSubtype {
		PolylineST        = 1,
		Box               = 2,
		Polygon           = 3,
		ArcBox            = 4,
		ImportedPictureBB = 5
	};

	enum EllipseSubtype {
		EllipseByRadii     = 1,
		EllipseByDiameters = 2,
		CircleByRadius     = 3,
		CircleByDiameter   = 4
	};

	enum LineStyle {
		DefaultLineStyle = -1,
		Solid            =  0,
		Dashed           =  1,
		Dotted           =  2,
		DashDotted       =  3,
		DashDoubleDotted =  4,
		DashTripleDotted =  5
	};

	enum Color {
		DefaultColor = -1,
		Black   =  0,
		Blue    =  1,
		Green   =  2,
		Cyan    =  3,
		Red     =  4,
		Magenta =  5,
		Yellow  =  6,
		White   =  7
		// TODO: fill in the rest of the colors
	};

	enum LaTeXFont {
		DefaultLaTeXFont = 0,
		Roman      = 1,	
		Bold       = 2,	
		Italic     = 3,	
		SansSerif  = 4,	
		Typewriter = 5	
	};

	enum PostScriptFont {
		DefaultPSFon                   = -1,
		TimesRoman                     =  0,
		TimesItalic                    =  1,
		TimesBold                      =  2,
		TimesBoldItalic                =  3,
		AvantGardeBook                 =  4,
		AvantGardeBookOblique          =  5,
		AvantGardeDemi                 =  6,
		AvantGardeDemiOblique          =  7,
		BookmanLight                   =  8,
		BookmanLightItalic             =  9,
		BookmanDemi                    = 10,
		BookmanDemiItalic              = 11,
		Courier                        = 12,
		CourierOblique                 = 13,
		CourierBold                    = 14,
		CourierBoldOblique             = 15,
		Helvetica                      = 16,
		HelveticaOblique               = 17,
		HelveticaBold                  = 18,
		HelveticaBoldOblique           = 19,
		HelveticaNarrow                = 20,
		HelveticaNarrowOblique         = 21,
		HelveticaNarrowBold            = 22,
		HelveticaNarrowBoldOblique     = 23,
		NewCenturySchoolbookRoman      = 24,
		NewCenturySchoolbookItalic     = 25,
		NewCenturySchoolbookBold       = 26,
		NewCenturySchoolbookBoldItalic = 27,
		PalatinoRoman                  = 28,
		PalatinoItalic                 = 29,
		PalatinoBold                   = 30,
		PalatinoBoldItalic             = 31,
		Symbol                         = 32,
		ZapfChanceryMediumItalic       = 33,
		ZapfDingbats                   = 34
	};

	enum Just {
		Left   = 0,
		Center = 1,
		Right  = 2,
	};

	enum FillPattern {
		NoFill      = -1,
		Filled      = 20,
		LDiag30     = 41,
		RDiag30     = 42,
		Hatch30     = 43,
		LDiag45     = 44,
		RDiag45     = 45,
		Hatch45     = 46,
		HBricks     = 47,
		VBricks     = 48,
		HLines      = 49,
		VLines      = 50,
		Hatch       = 51,
		HShingles1  = 52,
		HShingles2  = 53,
		VShingles1  = 54,
		VShingles2  = 55,
		BigScales   = 56,
		SmallScales = 57, 
		Circles     = 58,
		Hexagons    = 59,
		Octagons    = 60,
		HTreads     = 61,
		VTreads     = 62
	};

	enum JoinStyle {
		Miter = 0,
		RoundCap = 1,
		Bevel = 2
	};

	enum CapStyle {
		Butt       = 0,
		RoundJoin  = 1,
		Projecting = 2
	};

	Fig(ostream &);
	
	void reset();
	void pushSettings();
	void popSettings();

	void setSize(double, double);
	void setView(double, double, double, double);
		// These determine how big things are.  The size values are not enforced;
		// they mearly determine the size of the abstract units used to specify the
		// view.  By reversing the min and max for an axis, you can reflect
		// everything about that axis.

	void beginObject();
	void addPoint(double, double);
	void endPolyline();
	void endPolygon();
	
	void addCircle(double x, double y, double r);
		// r is in inches.

	void addText(double x, double y, string text);

	void setDepth(int);
	int getDepth();
	void increaseDepth();
	void decreaseDepth();

	void setFillPattern(FillPattern);
	void setFillShade(int);
	int getFill();

	void setJustification(Just);
	Just getJustification();

	void setLineStyle(LineStyle);
	LineStyle getLineStyle();

	void setDashLength(int);
	int getDashLength();

	void setLineThickness(int);
	int getLineThickness();

	void setPenColor(Color);
	Color getPenColor();

	void setFillColor(Color);
	Color getFillColor();

	void setColor(Color);
		// Shorthand to set the pen color and fill color together

	void nextColor();
		// Shorthand to setColor() to the next available color.

	void setJoinStyle(JoinStyle);
	JoinStyle getJoinStyle();

	void setCapStyle(CapStyle);
	CapStyle getCapStyle();
	
	void setForwardArrow(bool);
	bool getForwardArrow();

	void setBackwardArrow(bool);
	bool getBackwardArrow();

	static Color encodeColor(double, double, double);

	double toVerticalInches(double y);
	double toHorizontalInches(double x);

protected:
	static const int FigUnitsPerInch =  1200;

	ostream &out;

	bool sizeInitialized, viewInitialized;

	struct Parameters {
		int depth;
		int fill;
		Just just;
		LineStyle lineStyle;
		int dashLength;
		int lineThickness;
		Color penColor;
		Color fillColor;
		JoinStyle joinStyle;
		CapStyle capStyle;
		bool forwardArrow;
		bool backwardArrow;
	} p;

	struct Point {
		Point(int,int);
		int x;
		int y;
	};

	Point mapPoint(double x, double y);

	double width, height; // ...in inches.
	double min_x, max_x, min_y, max_y; // ... in arbitrary units.

	vector<Point> points;

	stack<Parameters> paramStack;

	void generatePolyline(PolylineSubtype);
	void generateEllipse(EllipseSubtype, Point, Point, Point, Point);

};


void showFig(char const * fileName);

#endif
