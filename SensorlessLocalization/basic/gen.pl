#!/usr/bin/perl -w

$PI = 3.14159;

$IMAX = 5;
$JMAX = 5;
$POLYS = ($IMAX+1) * ($JMAX+1) + 1;
$SIDES = 7;
$XDIM = $IMAX + 2;
$YDIM = $JMAX + 2;

print "$POLYS\n";
print "4 0 0 0 $YDIM $XDIM $YDIM $XDIM 0\n";

foreach $i (0..$IMAX) {
	foreach $j (0..$JMAX) {
		print "$SIDES \n";
		foreach $k (1..$SIDES) {
			$r = 0.25;
			printf ("%.2f ", ((($i+1.0)+ $r * cos(2 * $k * $PI / $SIDES))));
			printf ("%.2f\n", ((($j+1.0)+ $r * sin(2 * $k * $PI / $SIDES))));
		}
	}
}
