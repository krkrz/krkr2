use strict;
use XML::DOM;
use Jcode;
use Image::Size;

my $image_dir;

open FH , "./imgdir.pl";
my @content_all = <FH>;
my $content = join('', 	@content_all);
close FH;

eval $content;

my @keywords;  ;# keywords
my $unique = 1;

my $cv_nbsp = 0;

sub sjis
{
	my($text);
	$text = $_[0];
	$text = Jcode->new($text, "utf8")->sjis;
	$text =~ s/---yen---/\\/g;
	$text =~ s/---tilde---/\~/g;
	$text =~ s/---nami---/～/g;
	$text =~ s/&/&amp;/g;
	$text =~ s/</&lt;/g;
	$text =~ s/>/&gt;/g;
if($cv_nbsp)
{
	$text =~ s/ /&nbsp;/g;
	$text =~ s/\t/&nbsp;&nbsp;&nbsp;&nbsp;/g;
}
	$text =~ s/\"/&quot;/g;  #"
	return $text;
}

sub html
{
	my($text);
	$text = $_[0];
	$text =~ s/&/&amp;/g;
	$text =~ s/</&lt;/g;
	$text =~ s/>/&gt;/g;
if($cv_nbsp)
{
	$text =~ s/ /&nbsp;/g;
	$text =~ s/\t/&nbsp;&nbsp;&nbsp;&nbsp;/g;
}
	$text =~ s/\"/&quot;/g; #"
	return $text;
}

sub sjis_noquote
{
	my($text);
	$text = $_[0];
	$text = Jcode->new($text, "utf8")->sjis;
	$text =~ s/---yen---/\\/g;
	$text =~ s/---tilde---/\~/g;
	$text =~ s/---nami---/～/g;
	return $text;
}


sub get_title
{
	my $file = $_[0];
	my $fn;
	$fn = "..\\" . $file . ".xml";
	if(!open(LFH, $fn))
	{
		$fn = "..\\" . $file . ".tag";
		if(!open(LFH, $fn))
		{
			return "";
		}
	}
	my @all = <LFH>;
	my $all = join('', @all);
	close LFH;
	$all =~ /\<title\>(.*?)\<\/title\>/;
	return $1;
}

my $orgfile;
my $outfile;
my $a_target;
my $curtitle;
my $curplace;

sub gen_html
{
	my($node) = @_;

	my($type);

	$type = $node->getNodeType;
	if($type == ELEMENT_NODE)
	{
		my($name);
		$name = sjis($node->getNodeName);
		if($name eq "note")
		{
			print OH <<EOF;
<div class="note"><div class="notehead">&nbsp;&nbsp;note&nbsp;&nbsp;</div>
EOF

			foreach my $child ($node->getChildNodes) { &gen_html($child); }

			print OH <<EOF;
</div>
EOF
		}
		elsif($name eq "descimg")
		{
			my $title = sjis(($node->getElementsByTagName("dititle"))[0]->getFirstChild->getData);
			print OH "<div class=\"descimg\"";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "$title</div>";
		}
		elsif($name eq "kw")
		{
			$unique ++;
			my $word = $node->getFirstChild->getData;
			$word = sjis_noquote($word) . "\t" . "id$unique" . "\t" . $outfile . "\t";
			$word .= "$curplace";
			push (@keywords, $word);
			print OH "<a class=\"targanchor\" name=\"id$unique\" id=\"id$unique\">";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</a>";
		}
		elsif($name eq "bq")
		{
			print OH "<div class=\"bq\">";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</div>\n";
		}
		elsif($name eq "desc")
		{
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
		}
		elsif($name eq "img")
		{
			my $filename = $image_dir . $node->getAttribute("src");
			if(-e $filename)
			{
				my $w;
				my $h;
				($w, $h) = imgsize($filename);
				print OH "<img width=\"$w\" height=\"$h\" src=\"". sjis($node->getAttribute("src"))."\" />";
			}
			else
			{
				print OH "<img src=\"".sjis($filename)."\" />";
			}
		}
		elsif($name eq "ref")
		{
			my $link = $node->getFirstChild->getData;
			my $org = $link;
			$link =~ tr/\./_/;
			print OH "<a class=\"jump\" href=\"f_". $link. ".html\">$org</a>";
		}
		elsif($name eq "example")
		{
			my $org_cv_nbsp = $cv_nbsp;
			$cv_nbsp = 1;
			print OH "<code class=\"bq\"><span class=\"weak\">例:</span><br />";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</code>\n";
			$cv_nbsp = $org_cv_nbsp;
		}
		elsif($name eq "a")
		{
			print OH "<a class=\"jump\" href=\"" . sjis($node->getAttribute("href")) . "\">";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</a>";
		}
		elsif($name eq "r")
		{
			print OH "<br />\n";
		}
		elsif($name eq "tt")
		{
			print OH "<span class=\"script\">";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</span>";
		}
		elsif($name eq "link")
		{
			my $href = $node->getAttribute("href");
			my $title = get_title($href);
			if($title ne '')
			{
				print OH "<a target=\"$a_target\" class=\"jump\" href=\"${href}.html\">$title</a>";
			}
		}
		elsif($name eq "comlink")
		{
			my $href = $node->getAttribute("href");
			my $title = get_title($href);
			if($title ne '')
			{
				print OH "<font size=\"-1\"> ( → <a target=\"$a_target\" class=\"jump\" href=\"${href}.html\">$title</a> ) </font>";
			}
		}
		else
		{
			print OH "<$name>";
			foreach my $child ($node->getChildNodes) { &gen_html($child); }
			print OH "</$name>";
		}
	}
	elsif($type == TEXT_NODE)
	{
		my $text = $node->getData;
		$text =~ s/^\n//s;
		$text =~ s/\n$//s;
		$text = sjis($text);
		print OH $text;
	}
	elsif($type == DOCUMENT_NODE)
	{
		foreach my $child ($node->getChildNodes) { &gen_html($child); }
	}
}


my $topdesc;
my @members;

sub toplevel
{
	my($node) = @_;
	my($type);

	$type = $node->getNodeType;
	if($type == ELEMENT_NODE)
	{
		my($name);
		$name = sjis($node->getNodeName);
		if($name eq "desc")
		{
			;# top level description
			$topdesc = $node;
		}
		elsif($name eq "member")
		{
			;# members
			push(@members, $node);
		}
		elsif($name eq "title")
		{
			;# title
			$curtitle = sjis($node->getFirstChild->getData);
			$curplace = $curtitle . "クラス";
			print "title: $curtitle\n";
		}
		elsif($name eq "anchor")
		{
			$a_target = sjis($node->getAttribute("target"));
		}
		else
		{
			foreach my $child ($node->getChildNodes) { &toplevel($child); }
		}
	}
}

sub write_html_header
{
	my($title, $orgfile, $parent, $parenttitle) = @_;
	print OH <<EOF;
<?xml version="1.0" encoding="Shift_JIS"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html  xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<!-- generated by to_html.pl from $orgfile -->
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=Shift_JIS" />
	<title>$title</title>
	<meta name="author" content="W.Dee" />
	<meta http-equiv="Content-Style-Type" content="text/css" />
	<meta http-equiv="Content-Script-Type" content="text/javascript" />
	<link href="funcref.css" type="text/css" rel="stylesheet" title="クラスリファレンス用標準スタイル" />
	<link href="mailto:dee\@kikyou.info" rev="Made" />
	<link href="index.html" target="_top" rel="Start" title="トップページ" />
EOF

	if($parent ne "")
	{
		print OH "\t<link href=\"$parent\" title=\"$parenttitle\" rel=\"Parent\" />\n";
	}

	print OH <<EOF;
</head>
<body>
EOF
}

sub write_html_trailer
{
	print OH <<EOF;
	<script type="text/javascript" charset="Shift_JIS" src="documentid.js" ></script>
	<script type="text/javascript" charset="Shift_JIS" src="postcontent.js" ></script>
</body>
</html>
EOF
}

sub write_paragraph_header
{
	my($title) = @_;
	print OH <<EOF;
<h1>$title</h1><div class="para"><div>
EOF
}

sub write_paragraph_trailer
{
	print OH <<EOF;
</div></div>
EOF
}

sub getdata
{
	my($node, $name) = @_;
	my @list;
	@list = $node->getElementsByTagName($name);
	if($#list != -1)
	{
		my $first = $list[0]->getFirstChild;
		if(defined $first)
		{
			return sjis($first->getData);
		}
		else
		{
			return '';
		}
	}
	else
	{
		return '';
	}
}

sub member
{
	my($node) = @_;
	my @list;
	my $first;
	my($name) = sjis(($node->getElementsByTagName("name"))[0]->getFirstChild->getData);

	$outfile = 'f_'.$curtitle.'_'.$name.".html";

	$curplace = $curtitle . '.' . $name;

	open OH, ">$outfile";

	&write_html_header($name . ' - ' . getdata($node, 'shortdesc'), $orgfile, "f_${curtitle}.html", "$curtitleクラス");

	push @keywords, $name . "\t". "top". "\t". $outfile ."\t". $curtitle . "クラス";

	&write_paragraph_header('<span class="fheader">' . "<a name=\"". "top" . "\" id=\"". "top" . "\">".$curtitle . '.' . $name . '</a></span>');

	print OH "<dl>\n";

	print OH <<EOF;
<dt>機\x94\x5c/意味</dt>
<dd>
EOF
	print OH getdata($node, 'shortdesc');

	print OH <<EOF;
</dd>
EOF


	print OH <<EOF;
<dt>タイプ</dt>
<dd>
EOF
	print OH "<a class=\"jump\" href=\"f_$curtitle".".html\">$curtitleクラス</a>の";
	my $type = getdata($node, 'type');
	if($type eq 'constructor')
	{
		print OH 'コンストラクタ';
	}
	elsif($type eq 'method')
	{
		print OH 'メソッド';
	}
	elsif($type eq 'property')
	{
		print OH 'プロパティ';
		my $access = getdata($node, 'access');
		if($access eq 'r')
		{
			print OH ' (読み出し専用)';
		}
		elsif($access eq 'r/w' || $access eq 'w/r')
		{
			print OH ' (読み書き可能)';
		}
		elsif($access eq 'w')
		{
			print OH ' (書き込み専用)';
		}
	}
	elsif($type eq 'event')
	{
		print OH 'イベント';
	}
	print OH "<br />\n";
	print OH <<EOF;
</dd>
EOF

	if($type eq 'method' || $type eq 'constructor' || $type eq 'event')
	{
		print OH <<EOF;
<dt>\x8d\x5c文</dt>
<dd>
EOF
		print OH "<span class=\"funcdecl\">$name(";
		@list = $node->getElementsByTagName('argitem');
		$first = 1;
		foreach my $arg (@list)
		{
			if(!$first) { print OH ", "; }
			$first = 0;
			print OH "<span class=\"arg\">";
			print OH getdata($arg, 'name');
			print OH "</span>";
			my $def = getdata($arg, 'default');
			if($def ne '')
			{
				print OH "<span class=\"defarg\">=";
				print OH "<span class=\"defargval\">$def</span></span>";
			}
		}
		print OH ")</span><br />\n";
		print OH <<EOF;
</dd>
EOF

		print OH <<EOF;
<dt>引数</dt>
<dd>
EOF
		if($#list == -1)
		{
			print OH "なし<br />\n";
		}
		else
		{
			print OH <<EOF;
<table rules="all" frame="box" cellpadding="3" summary="$name の引数">
EOF
			foreach my $arg (@list)
			{
				print OH "<tr><td valign=\"top\"><span class=\"argname\">";
				print OH getdata($arg, 'name');
				print OH "</span></td>\n";
				print OH "<td>";
				&gen_html($arg->getElementsByTagName("desc"));
				print OH "</td></tr>\n";
			}

			print OH <<EOF;
</table>
EOF
		}
		print OH <<EOF;
</dd>
EOF

	}

	if($type eq 'method' || $type eq 'constructor')
	{

		print OH <<EOF;
<dt>戻り値</dt>
<dd>
EOF
		my $result = getdata($node, 'result');
		if($result eq '')
		{
			print OH "なし (void)<br />\n";
		}
		else
		{
			&gen_html($node->getElementsByTagName("result", 0));
		}

		print OH <<EOF;
</dd>
EOF
	}

	print OH <<EOF;
<dt>説明</dt>
<dd>
EOF

	&gen_html($node->getElementsByTagName("desc", 0));


	print OH <<EOF;
</dd>
EOF

	@list = $node->getElementsByTagName('ref', 0);

	if($#list != -1)
	{
		print OH <<EOF;
<dt>参照</dt>
<dd>
EOF
		foreach my $ref (@list)
		{
			my $targ = sjis($ref->getFirstChild->getData);
			my $link = $targ;
			$link =~ tr/\./_/;
			print OH "<a class=\"jump\" href=\"f_". $link. ".html\">$targ</a><br />\n";
		}
		print OH <<EOF;
</dd>
EOF
	}

	print OH "</dl>\n";

	&write_paragraph_trailer;

	&write_html_trailer;
}

sub document
{

	my($node) = @_;
	my(@names);

	@members = ();

	&toplevel($node->getElementsByTagName("doc"));

	;# create class summary

	open OH, ">$outfile";

	&write_html_header($curtitle, $orgfile, '', '');

	push @keywords, $curplace . "概要". "\t". "top". "\t". $outfile ."\t". $curplace;

	&write_paragraph_header("<a name=\"". "top" . "\" id=\"". "top" . "\">" . $curtitle . "</a>");

	&gen_html($topdesc);

	&write_paragraph_trailer;

	;# create member list

	&write_paragraph_header("メンバ");

	@names = ();

	print OH <<EOF;
<dl>
<dt>コンストラクタ</dt>
<dd>
EOF
	foreach my $member (@members)
	{
		if(sjis(($member->getElementsByTagName("type"))[0]->getFirstChild->getData) eq "constructor")
		{
			push(@names, sjis(($member->getElementsByTagName("name"))[0]->getFirstChild->getData));
		}
	}
	@names = sort @names;
	if($#names == -1)
	{
		print OH <<EOF;
<span class="weak">なし</span>
EOF
	}
	else
	{
		foreach my $member (@names)
		{
			print OH "<a class=\"jump\" href=\"f_". $curtitle.'_'.$member.".html\">$member</a><br />\n";
		}
	}

	print OH <<EOF;
</dd>
EOF

	@names = ();

	print OH <<EOF;
<dt>メ\x83\x5cッド</dt>
<dd>
EOF
	foreach my $member (@members)
	{
		if(sjis(($member->getElementsByTagName("type"))[0]->getFirstChild->getData) eq "method")
		{
			my $name = sjis(($member->getElementsByTagName("name"))[0]->getFirstChild->getData);
			push(@names, "<a class=\"jump\" href=\"f_". $curtitle.'_'.$name.".html\">$name</a> ( ".
				sjis(($member->getElementsByTagName("shortdesc"))[0]->getFirstChild->getData)." )<br />\n");
		}
	}
	@names = sort @names;
	if($#names == -1)
	{
		print OH <<EOF;
<span class="weak">なし</span>
EOF
	}
	else
	{
		foreach my $member (@names)
		{
			print OH $member;
		}
	}

	print OH <<EOF;
</dd>
EOF

	@names = ();

	print OH <<EOF;
<dt>プロパティ</dt>
<dd>
EOF
	foreach my $member (@members)
	{
		if(sjis(($member->getElementsByTagName("type"))[0]->getFirstChild->getData) eq "property")
		{
			my $name = sjis(($member->getElementsByTagName("name"))[0]->getFirstChild->getData);
			push(@names, "<a class=\"jump\" href=\"f_". $curtitle.'_'.$name.".html\">$name</a> ( ".
				sjis(($member->getElementsByTagName("shortdesc"))[0]->getFirstChild->getData)." )<br />\n");
		}
	}
	@names = sort @names;
	if($#names == -1)
	{
		print OH <<EOF;
<span class="weak">なし</span>
EOF
	}
	else
	{
		foreach my $member (@names)
		{
			print OH $member;
		}
	}

	print OH <<EOF;
</dd>
EOF

	@names = ();

	print OH <<EOF;
<dt>イベント</dt>
<dd>
EOF
	foreach my $member (@members)
	{
		if(sjis(($member->getElementsByTagName("type"))[0]->getFirstChild->getData) eq "event")
		{
			my $name = sjis(($member->getElementsByTagName("name"))[0]->getFirstChild->getData);
			push(@names, "<a class=\"jump\" href=\"f_". $curtitle.'_'.$name.".html\">$name</a> ( ".
				sjis(($member->getElementsByTagName("shortdesc"))[0]->getFirstChild->getData)." )<br />\n");
		}
	}
	@names = sort @names;
	if($#names == -1)
	{
		print OH <<EOF;
<span class="weak">なし</span>
EOF
	}
	else
	{
		foreach my $member (@names)
		{
			print OH $member;
		}
	}

	print OH <<EOF;
</dd>
EOF

	print OH "</dl>\n";

	&write_paragraph_trailer;

	&write_html_trailer;

	close OH;

	;# create each member description

	foreach my $member (@members)
	{
		&member($member);
	}

}


sub process
{
	my $fn = $_[0];
	my $of = $fn;
	$of =~ s/\.xml/\.html/;

	$orgfile = $fn;
	$outfile = $of;
	$a_target = 'main';
	open FH , $fn;
	my @content_all = <FH>;
	my $content = join('', 	@content_all);
	close FH;

	my $parser = new XML::DOM::Parser;

	$content =~ s/～/---nami---/g;
	$content = Jcode->new($content, "sjis")->euc;
	$content =~ s/\\/---yen---/g;
	$content =~ s/\~/---tilde---/g;
	$content = Jcode->new($content, "euc")->sjis;
	$content =~ s/Shift_JIS/x-sjis-unicode/;
	my $doc = $parser->parse($content);

	&document($doc);
}

my @list = <*.xml>;

foreach my $each (@list)
{
	print $each, "\n";
	&process($each);
}

open OH, ">keys.txt";
print OH join("\n", @keywords);
print OH "\n";

