open FH, "../.svn/entries";
undef $/;

$revision = "<unknown svn revision>";

$lines = <FH>;

if($lines =~ /\<entry(.*?)name=\"\"(.*?)\/\>/s)
{
	$lines = "$1\t$2";

	$u = $1 if($lines =~ /url=\"(.*?)\"/s);
	$r = $1 if($lines =~ /revision=\"(\d+)\"/s);

	if($u ne "" && $r ne "")
	{
		$revision = "SVN rev=$r url=$u";
	}
}


open FH, ">svn_revision.h";


print FH <<EOF;
/* this file is automatically updated by update_svn_revision.pl, called from release.bat */

#define TVP_SVN_REVISION "$revision"
#define TVP_SVN_REVISION_NUMBER "$r"

EOF

