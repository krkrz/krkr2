# tags.database.tml を HTML 形式に変換し、tags.html に出力する
# perl スクリプト

# 解釈がいい加減なので注意

@keywords = ();

# まずはファイル一個よみこんぢまえ

open FH,"tags.database.tml";
@all=<FH>;
$all=join('',@all);


# 行頭と行末の空白文字を削除し、行を連結
sub cliptext
{
	$kdata=$_[0];
	$kdata=~ s/^\s*(.+)\s*$/$1/gm;
	$kdata=~ s/\n//g;
	$kdata=~ s/\s*$//g;
	return $kdata;
}

# タグ分解
# tml では同じ名前のタグが入れ子になることはないので
# そういう解析はしない

sub taganalysis
{
	$data=$_[0];
	$tag=$_[1];
	local(@contents);
	@contents=();
	while($data =~ m/(\<$tag[^\>]+\>)/i)
	{
		$taginfo=$1;
		$data2=$';
		$data2=~ /\<\/$tag\>/;
		$content=$`;
		push(@contents,$taginfo.$content);
		$data=$';
	}
	return @contents;
}


@tags=&taganalysis($all,"tag");

foreach $tagcontent (@tags)
{
	# tag をまた解析
	$tagcontent=~ /\<tag name\=[\'\"]([^\'\"]+)[\'\"]\>/i;

	$tagname=$1;
	$tagcontent=$';  #'

	# shortinfo の取得
	$temp=$tagcontent;
	$temp=~ /\<shortinfo\>/i;
	$temp=$'; #'
	$temp=~ /\<\/shortinfo\>/i;
	$shortinfo=$`;

	# shortinfo の格納
	$tagdata{$tagname}{"shortinfo"}=&cliptext($shortinfo);

	# group の取得
	$temp=$tagcontent;
	$temp=~ /\<group\>/i;
	$temp=$';
	$temp=~ /\<\/group\>/i;
	$group=$`;  #'

	# group の格納
	$tagdata{$tagname}{"group"}=&cliptext($group);

	# remarks の取得
	$temp=$tagcontent;
	$temp=~ /\<remarks\>/i;
	$temp=$';  #'
	$temp=~ /\<\/remarks\>/i;
	$remarks=$`;


	# remarks の格納
	$tagdata{$tagname}{"remarks"}=&cliptext($remarks);

	# example の取得
	$temp=$tagcontent;
	if($temp=~ /\<example\>/i)
	{
		$temp=$';  #'
		$temp=~ /\<\/example\>/i;
		$example=$`;

		# example の格納
		$tagdata{$tagname}{"example"}=&cliptext($example);
	}


	# attribs の取得
	$temp=$tagcontent;
	if($temp=~ /\<attribs\>/i)
	{
		$temp=$';  #'
		$temp=~ /\<\/attribs\>/i;
		$attribscontent=$`;

		# attrib の分解
		$no=0;
		@attribs=&taganalysis($attribscontent,"attrib");
		foreach $attribscontent (@attribs)
		{
			# attrib 名前の取得
			$attribscontent=~ /\<attrib name\=[\'\"]([^\'\"]+)[\'\"]/i;  #'
			@attribnames=split(/\,/,$1);


			# shortinfo の取得
			$attribshortinfo="";
			$temp=$attribscontent;
			$temp=~ /\<shortinfo\>/i;
			$temp=$';  #'
			$temp=~ /\<\/shortinfo\>/i;
			$attribshortinfo=$`;

			# required の取得
			$attribrequired="";
			$temp=$attribscontent;
			$temp=~ /\<required\>/i;
			$temp=$';  #'
			$temp=~ /\<\/required\>/i;
			$attribrequired=$`;

			# format の取得
			$attribformat="";
			$temp=$attribscontent;
			$temp=~ /\<format\>/i;
			$temp=$';  #'
			$temp=~ /\<\/format\>/i;
			$attribformat=$`;

			# info の取得
			$attribinfo="";
			$temp=$attribscontent;
			$temp=~ /\<info\>/i;
			$temp=$';  #'
			$temp=~ /\<\/info\>/i;
			$attribinfo=$`;


			# データの格納
			foreach $attribname(@attribnames)
			{

				%data=();

				$data{"shortinfo"}=
						&cliptext($attribshortinfo);

				$data{"required"}=
					&cliptext($attribrequired);

				$data{"format"}=
						&cliptext($attribformat);
				$data{"info"}=
						&cliptext($attribinfo);

				$data{"nam__e"}=$attribname;

				$tagdata{$tagname}{"attribs_data_".$no}=
					join("__SPLIT__",%data);
				$no++;
			}
		}
	}
}



# データの吐き出し


@data=<DATA>;
print @data;


sub conv_html
{
	$data=$_[0];
	$data=~ s/\<ref\s+tag\=[\"\']([^\"\']+)[\"\']\>/\<a class=\"jump\" href=\"\#$1\"\>/gi;
	$data=~ s/\<\/ref\>/\<\/a\>/gi;
	$data=~ s/<br>/<br \/>/gi;
	$data =~ s/<tt>(.*?)<\/tt>/<span class=\"script\">$1<\/span>/gsi;
	return $data;
}


@h_tagdata=%tagdata;

@outdata=();


@genredata=();

for($i=0;$i<=$#h_tagdata;$i+=2)
{
	$od = "";
	$od.="\n";
	$current_tag = $h_tagdata[$i];
	push @keywords, $h_tagdata[$i] . "\t" . $h_tagdata[$i] . "\t". "Tags.html" . "\t". "タグリファレンス";
	$od.="<h1><a name=\"$h_tagdata[$i]\" id=\"$h_tagdata[$i]\" class=\"targanchor\"><span class=\"b\">$h_tagdata[$i]</span> ( ".$h_tagdata[$i+1]{"shortinfo"}." )</a></h1>\n";
	$od.="<div class=\"taggenre\">".$h_tagdata[$i+1]{"group"}."</div>\n";
	$od.="<div class=\"para\"><div>";

	$gd=$h_tagdata[$i+1]{"group"}."__SPLIT__".$h_tagdata[$i]."__SPLIT__".
		$h_tagdata[$i+1]{"shortinfo"};

	push(@genredata,$gd);

	if($h_tagdata[$i+1]{"attribs_data_0"} ne "")
	{


		@h_data=%data;

		$od.="<table class=\"tagparams\" frame=\"box\" rules=\"all\" summary=\"タグ " . $h_tagdata[$i] ." (" . $h_tagdata[$i+1]{"shortinfo"}.") の属性の一覧\">";
		$od.="<thead><tr>";
		$od.="<td>属性</td><td>必須?</td>";
		$od.="<td>値</td><td>説明</td>";
		$od.="</tr></thead><tbody>\n";

		$no=0;
		
		while(1)
		{
			%data=split(/__SPLIT__/,$h_tagdata[$i+1]{"attribs_data_".$no});
		
			$od.="<tr>";
			$od.="<td class=\"tagattribname\"><a class=\"targanchor\" name=\"". $current_tag . "_" . $data{"nam__e"} . "\" id=\"". $current_tag . "_" . $data{"nam__e"} . "\">";
			$od.=$data{"nam__e"};
			push @keywords, $data{"nam__e"} . "\t" . $current_tag . "_" . $data{"nam__e"} . "\t" .
				"Tags.html" . "\t". "タグリファレンス-" . $current_tag;
			$od.="</a></td>";
			$od.="<td class=\"tagattribrequired\">";
			if($data{"required"} eq "yes")
			{
				$od.="<span class=\"tagrequiredattrib\">";
			}
			$od.=$data{"required"};
			if($data{"required"} eq "yes")
			{
				$od.="</span>";
			}
			$od.="</td>";
			$od.="<td class=\"tagattribformat\">";
			$od.=$data{"format"};
			$od.="</td>";
			$od.="<td class=\"tagattribdesc\">";
			$od.=&conv_html($data{"info"});
			$od.="</td>";
			$od.="</tr>\n";

			$no++;
			
			last if($h_tagdata[$i+1]{"attribs_data_".$no} eq "");
		}
		$od.="</tbody></table>";
	}
	
	$od.= "<div class=\"tagremarks\">". &conv_html($h_tagdata[$i+1]{"remarks"}) ."</div>\n";


	if($h_tagdata[$i+1]{"example"} ne "")
	{
		$od.="<div class=\"tagexample\"><code class=\"bq\"><span class=\"weak\">例:</span><br />\n";
		$od.=&conv_html($h_tagdata[$i+1]{"example"})."\n";
		$od.="</code></div>\n";
	}

	$od.="<div class=\"toindex\"><a class=\"jump\" href=\"#genre\"><span class=\"toindexanchor\">ジャンル・タグ一覧に戻る</span></a></div>";
	$od.="</div></div>\n";

	push(@outdata,$od);
}

print "<div class=\"para\"><div>";

$genre="";
$phase=0;
foreach $data( sort @genredata)
{
	@dat=split(/__SPLIT__/,$data);
	if($genre ne $dat[0])
	{
		print "</tbody></table>\n" if $genre ne "";
		$genre=$dat[0];
		print "<br />\n<div class=\"taggenrehead\">$genre</div><br />\n";
		print "<table class=\"taglist\" summary=\"$genre タグ一覧\"><tbody>\n";
	}
	print "<tr>";
	if($phase)
	{
		print "<td class=\"taglistodd\">";
	}
	else
	{
		print "<td class=\"taglisteven\">";
	}
	print "<span class=\"taglistlink\"><a class=\"jump\" href=\"#$dat[1]\">$dat[1]</a></span>";
	print "</td>";
	if($phase)
	{
		print "<td class=\"taglistodd\">";
	}
	else
	{
		print "<td class=\"taglisteven\">";
	}
	$phase ^=1;
	print "( $dat[2] )";
	print "</td></tr>\n";
}

print "</tbody></table><br /><br /></div></div>\n";

foreach $data (sort @outdata)
{
	print $data;
}


print <<EOF;
	<script type="text/javascript" charset="Shift_JIS" src="documentid.js" ></script>
	<script type="text/javascript" charset="Shift_JIS" src="postcontent.js" ></script>
EOF


print "</body></html>\n";

push keywords, "タグの概要\ttag_overview\tTags.html\tタグリファレンス";
push keywords, "コマンド行\ttag_command\tTags.html\tタグリファレンス";
push keywords, "cond 属性\ttag_cond_attrib\tTags.html\tタグリファレンス";
push keywords, "タグリファレンス\ttags\tTags.html\tタグリファレンス";

open OH, ">keys.txt";
print OH join("\n", @keywords);
print OH "\n";


__DATA__
<?xml version="1.0" encoding="Shift_JIS"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html  xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=Shift_JIS" />
	<title>タグリファレンス</title>
	<meta name="author" content="W.Dee" />
	<meta http-equiv="Content-Style-Type" content="text/css" />
	<meta http-equiv="Content-Script-Type" content="text/javascript" />
	<link href="browser.css" type="text/css" rel="stylesheet" title="吉里吉里関連リファレンス用標準スタイル" />
	<link href="mailto:dee@kikyou.info" rev="Made" />
	<link href="index.html" target="_top" rel="Start" title="トップページ" />
</head>
<body>

<h1><a name="tags" id="tags">タグリファレンス</a></h1>
<div class="para"><div>　KAG で使用可能なタグのリファレンスです。</div></div>
<h1><a name="tag_overview" id="tag_overview">タグの概要</a></h1>
<div class="para"><div>
　タグは、[ ] の間に囲まれたものです。基本的に半角英数小文字で記述します。
　<span class="script">[</span> の次にすぐにタグ名を書きます。たとえば、trans
というタグであれば <span class="script">[trans</span> となります。<br />
　タグには、タグのオプションを指定するために属性というものがあります。なにも属性を指定しなくていい場合は、タグ名のあとに <span class="script">]</span> を書いてタグは終わりです。たとえば
<span class="script">[ct]</span>
となります。<br />
　属性を指定する場合は、タグ名のあとや、前の属性のあとには必ず半角スペースを空けて、属性名を書きます。属性名の後には
= を書き、その後に属性の値を書きます。すべての属性を書いたら <span class="script">]</span>
でタグを閉じます。<br />
　たとえば、trans タグに time という属性を書きたい場合は、<span class="script">[trans time=0]</span> とします。また、さらに rule=trans vague=1 という属性を書きたい場合は
<span class="script">[trans time=0 rule=trans vague=1]</span> とします。<br />
　属性の値は "" で囲んでも囲まなくても同じです。たとえば、<span class="script">[trans time="0"]</span>と <span class="script">[trans time=0]</span>
は同じです。ただし、属性の値に空白を含むような場合は、"" で囲まなければなりません。たとえば、<span class="script">[font face="ＭＳ Ｐゴシック"]</span> です。<br />
　また、属性の値に対し、&amp; を最初につけると、その後に書いたものを
TJS 式として評価し、その結果を属性の値とします。たとえば、<span class="script">[trans time=&amp;f.clearTime]</span> とすると、<span class="script">f.clearTime</span> という変数の内容が、time
属性の値となります。<br />
</div></div>
<h1><a name="tag_command" id="tag_command">コマンド行</a></h1>
<div class="para"><div>
　コマンド行は、@ で始まり、その次にタグ名と属性を記述するものです。<br />
　一つの行には一つのタグのみを書くことができます。<br />
　以下の二つの行は等価になります。<br />
<br />
<div class="bq">
[trans time=0 rule=trans vague=1]<br />
@trans time=0 rule=trans vague=1<br />
</div>
</div>
</div>
<h1><a name="tag_cond_attrib" id="tag_cond_attrib">cond 属性</a></h1>
<div class="para"><div>
　macro endmacro if else elsif endif ignore endignore iscript endscript のタグをのぞき、すべてのタグに cond 属性があります。<br />
　cond 属性には TJS式 を指定し、この式を評価した結果が真の時のみにそのタグが実行されます。偽の時はタグは実行されません。<br />
<br />
例 :<br />
<div class="bq">
[l cond=f.noskip]<br />
; ↑ f.noskip が真の時のみ l タグを実行<br />
</div>
</div>
</div>

<h1><a name="genre" id="genre" class="targanchor">ジャンル・タグ一覧</a></h1>


