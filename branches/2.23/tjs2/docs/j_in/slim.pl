use strict;
use XML::DOM;
use Jcode;
use Image::Size;

undef $/;
sub process
{
	my $fn = $_[0];

	open FH , $fn;
	my $content = <FH>;
	close FH;

	$content =~ s/(<dl>\s*?)<r\/>/$1/sg;
	$content =~ s/<r\/>(\s*?<\/dl>)/$1/sg;

	$content =~ s/(<\/dt>\s*?)<r\/>/$1/sg;

	$content =~ s/<r\/>(\s*?)<\/dt>/$1<\/dt>/sg;
	$content =~ s/<\/dd>(\s*?)<r\/>/<\/dd>$1/sg;

	$content =~ s/<r\/>(\s*?)<\/dd>/$1<\/dd>/sg;
	$content =~ s/<\/dd>(\s*?)<r\/>/<\/dd>$1/sg;

	open FH, ">$fn";
	print FH $content;
	close FH;
}


my @list;
@list = <*.xml>;

foreach my $each (@list)
{
	print $each, "\n";
	&process($each);
}
