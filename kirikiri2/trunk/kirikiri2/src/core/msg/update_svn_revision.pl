undef $/;


$newest_rev = 0;

sub check_revision
{
	local($fn, $content, $entry);
	$fn = $_[0];
	open FH, "$fn";
	$content = <FH>;
	while($content =~ /\<entry.*?revision=\"(\d+)\".*?\/\>/gs)
	{
		;#"
		$newest_rev = $1 if($newest_rev < $1);
	}
}


sub process_dir
{
	local($dir, @dirlist, $file, $w, $h, $fn, $newfn, $content, $ext, $md5);
	$dir = $_[0];


	if(opendir(DH, "$dir"))
	{
		@dirlist = readdir DH;
		closedir(DH);
		foreach $file (@dirlist)
		{
			if($file eq '.svn')
			{
				;# a svn meta-information directory
				$fn = "$dir/$file/entries";
				&check_revision($fn);
			}
			elsif($file !~ /^\./)
			{
				$fn = "$dir/$file";
				if(!&process_dir($fn))
				{
					;# a file
				}
			}
		}
		return 1;
	}
	return 0;
}

process_dir('..');

$newest_rev = "unknown revision" if($newest_rev == 0);

open FH, "../.svn/entries";

$revision = "<unknown svn revision>";

$lines = <FH>;

if($lines =~ /\<entry(.*?)name=\"\"(.*?)\/\>/s)
{
	;# "
	$lines = "$1\t$2";

	$u = $1 if($lines =~ /url=\"(.*?)\"/s);
	;# "

	if($u ne "")
	{
		$revision = "SVN rev=$newest_rev url=$u";
	}
}


open FH, ">svn_revision.h";


print FH <<EOF;
/* this file is automatically updated by update_svn_revision.pl, called from release.bat */

#define TVP_SVN_REVISION "$revision"
#define TVP_SVN_REVISION_NUMBER "$newest_rev"

EOF

