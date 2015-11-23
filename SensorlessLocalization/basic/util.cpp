#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <string>

string formatString(const char *fmt, ...) {

	// Do a bunch of formatting.  This part shamelessly stolen from the printf
	// man page.

   /* Guess we need no more than 100 bytes. */
	 int n, size = 100;
	 char *p;
	 va_list ap;
	 if ((p = (char*)malloc (size)) == NULL)
			return NULL;
	 while (1) {
			/* Try to print in the allocated space. */
			va_start(ap, fmt);
			n = vsnprintf (p, size, fmt, ap);
			va_end(ap);
			/* If that worked, return the string. */
			if (n > -1 && n < size)
				break;
			/* Else try again with more space. */
			if (n > -1)    /* glibc 2.1 */
				 size = n+1; /* precisely what is needed */
			else           /* glibc 2.0 */
				 size *= 2;  /* twice the old size */
			if ((p = (char*)realloc (p, size)) == NULL)
				assert(false);
	 }
	 
	 // Make it into a nice string and return it.
		string r = string(p);
		free(p);
		return r;
}


void eatComments(ostream& out, istream &in) {
// Passes lines between two streams, stripping out all of the lines starting
// with a pound sign (#).  A strstream might be a good choice for the out
// parameter.
	const int bufferSize = 1024;
	char buf[bufferSize];

	while(in) {
		in.getline(buf, bufferSize);
		if(buf[0] != '#') {
			out << buf << endl;
		}
	}
}

int argmin3(double a, double b, double c) {
	if(a <= b && b <= c) return 1;
	if(b <= a && b <= c) return 2;
	return 3;

}

double *canonicalColor(int i) {
// Successive calls to this function return different "nice-looking" colors.
	const int number = 7;
	double static colors[7][3] = {
		{ 1, 0, 0 }, // red
		{ 0, 1, 0 }, // green
		{ 0, 0, 1 }, // blue
		{ 1, 0, 1 }, // magenta
		{ 1, 1, 0 }, // yellow
		{ 0, 1, 1 }, // cyan
		{ 0, 0, 0 }, // black
	};
	/*
		{ 0.5, 0, 0 }, // redish
		{ 0, 0.5, 0 }, // greenish
		{ 0, 0, 0.5 }, // blueish
		{ 0.5, 0, 0.5 }, // magentaish
		{ 0.5, 0.5, 0 }, // yellowish
		{ 0, 0.5, 0.5 }, // cyanish
	};
	*/

	return colors[i % number];
}
