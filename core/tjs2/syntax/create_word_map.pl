$input_file = "dp_wordtable.txt";

open FH, $input_file or die;

;# �P��t�@�C�������ׂēǂݍ���

%words = ();

while($line = <FH>)
{
	chomp $line;
	if($line =~ /^\s*(.*?)\s+(.*?)\s+(.*)/)
	{
		$words{"$1\b"} = "$2\t$3";
	}
}

;# �P����������Ƀ\�[�g�A�؂� switch ���ō\������

sub gen
{
	local($padding, $list, %list, $level, $i, $fc,
		$nfc, $cnt, @names, $name, %newlist, $v, $tok, $caselist, $deflist, $ret);
	$level = $_[0];
	$list  = $_[1];
	%list = %$list;
	@names = sort keys %list;
	$padding = " " x $level;

	if($#names == 0 && $names[0] eq "\b")
	{
		;# �I����������Ȃ��A�����[
		($v, $tok) = split("\t", $list{"\b"});
		return "${padding} if(!TJS_iswalpha(InputPointer[$level])) ".
			"{ InputPointer += $level; yylex->val = $v; return $tok; }\n"
	}

	;# �ŏ��̈ꕶ�����擾

	$fc = '';
	%newlist = ();
	$cnt = 0;


	$ret = '';

	$ret .= "${padding}switch(InputPointer[$level])\n";
	$ret .= "${padding}{\n";

	$caselist = '';
	$deflist = '';

	foreach $name (@names)
	{
		$nfc = substr($name, 0, 1);
		if($fc ne $nfc)
		{
			if($cnt)
			{
				if($fc eq "\b")
				{
					;# ���[
					$deflist .= "${padding}default:\n";
					($v, $tok) = split("\t", $list{"\b"});
					$deflist .= "${padding} if(!TJS_iswalpha(InputPointer[$level])) ".
						"{ InputPointer += $level; yylex->val = $v; return $tok; }\n";
				}
				else
				{
					$caselist .= "${padding}case TJS_W('$fc'):\n";
					if($fc =~ /[a-z]/)
					{
						$caselist .= "${padding}case TJS_W('".uc $fc."'):\n";
					}
					$caselist .= &gen($level + 1, \%newlist);
					$caselist .= "${padding} break;\n";
				}
			}
			$fc = $nfc;
			%newlist = ();
			$cnt = 0;
		}
		$newlist{substr($name,1)} = $list{$name};
		$cnt ++;
	}

	if($cnt)
	{
		if($fc eq "\b")
		{
			;# ���[
			$deflist .= "${padding}default:\n";
			($v, $tok) = split("\t", $list{"\b"});
			$deflist .= "${padding} InputPointer += $level; yylex->val = $v; return $tok;\n";
		}
		else
		{
			$caselist .= "${padding}case TJS_W('$fc'):\n";
			if($fc =~ /[a-z]/)
			{
				$caselist .= "${padding}case TJS_W('".uc $fc."'):\n";
			}
			$caselist .= &gen($level + 1, \%newlist);
			$caselist .= "${padding} break;\n";
		}
	}

	return $ret.$caselist.$deflist."${padding}}\n";
}

print <<EOF;
/*---------------------------------------------------------------------------*/
/*
	TJS2 Script Engine
	Copyright (C) 2000-2009 W.Dee <dee\@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*
	Date/time string parser lexical analyzer word cutter.

	This file is always generated from syntax/$input_file by
	syntax/create_word_map.pl. Modification by hand will be lost.

*/

EOF

print &gen(0, \%words);
