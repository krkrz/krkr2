use strict;
use XML::DOM;
use Jcode;
use Image::Size;


sub process
{
	my $fn = $_[0];

	open FH , $fn;
	my @content_all = <FH>;
	my $content = join('', 	@content_all);
	close FH;

	$content =~ s/Shift_JIS/x-sjis-unicode/;

	my $parser = new XML::DOM::Parser;
	my $doc = $parser->parse($content);
}


my @list;
if($ARGV[0] eq "f_")
{
	@list = <f_*.html>;
}
else
{
	@list = <*.html>;
}

foreach my $each (@list)
{
	print $each, "\n";
	&process($each);
}
