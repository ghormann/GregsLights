#!/usr/bin/perl

use strict;

my $targetHeight=46;
my $dir = "full";
opendir(DIR, "$dir") || die("Unable to read directory: $dir");
my @files = readdir(DIR);
closedir(DIR);

foreach my $file (@files) {
	my $full = "$dir/$file";
	next if (-d $full);
	print "$full\n";
	$file =~ /(.*)\./;
	my $newName = "$1_64_%d.png";

	my $width = `convert $full -print "%w" /dev/null`;
	my $height = `convert $full -print "%h" /dev/null`;
	my $test = $height * 2;
	my $targetWidth = $width * ($targetHeight/$height);
	my $targetWidth = int($targetWidth + 0.5);
	print "Target: $targetWidth, $targetHeight\n";
	my $cmd = "convert \"$full\" -colorspace RGB -depth 8 -define png:color-type=2 -resize \"${targetWidth}x$targetHeight\" \"$newName\"";
	print "$cmd\n";
	system ($cmd);
}
