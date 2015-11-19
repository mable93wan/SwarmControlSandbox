#!/usr/bin/perl

# For now, this will only work on FIGs that contain only one polyline and
# nothing else.

while($line = <>) {
	if(($numpts) = ($line =~ /2( ([^ ])*){15}(.*)/)) {
		print "1 ".($numpts-1)."\n";
		$/ = " ";
		for $i (1..$numpts-1) {
			do { ($ptx) = (<> =~ /([0-9]*)/); } while($ptx == "") ;
			($pty) = (<> =~ /([0-9]*)/);
			print "$ptx $pty\n";
		}
	}


}
