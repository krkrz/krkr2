# tags.database.tml を tag database 形式に変換し、出力する
# perl スクリプト

# 解釈がいい加減なので注意

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
	$tagcontent=$';

	# shortinfo の取得
	$temp=$tagcontent;
	$temp=~ /\<shortinfo\>/i;
	$temp=$';
	$temp=~ /\<\/shortinfo\>/i;
	$shortinfo=$`;

	# shortinfo の格納
	$tagdata{$tagname}{"shortinfo"}=&cliptext($shortinfo);


	# remarks の取得
	$temp=$tagcontent;
	$temp=~ /\<remarks\>/i;
	$temp=$';
	$temp=~ /\<\/remarks\>/i;
	$remarks=$`;


	# remarks の格納
	$tagdata{$tagname}{"remarks"}=&cliptext($remarks);

	# example の取得
	$temp=$tagcontent;
	if($temp=~ /\<example\>/i)
	{
		$temp=$';
		$temp=~ /\<\/example\>/i;
		$example=$`;

		# example の格納
		$tagdata{$tagname}{"example"}=&cliptext($example);
	}


	# attribs の取得
	$temp=$tagcontent;
	if($temp=~ /\<attribs\>/i)
	{
		$temp=$';
		$temp=~ /\<\/attribs\>/i;
		$attribscontent=$`;

		# attrib の分解
		$no=0;
		@attribs=&taganalysis($attribscontent,"attrib");
		foreach $attribscontent (@attribs)
		{
			# attrib 名前の取得
			$attribscontent=~ /\<attrib name\=[\'\"]([^\'\"]+)[\'\"]/i;
			@attribnames=split(/\,/,$1);


			# shortinfo の取得
			$attribshortinfo="";
			$temp=$attribscontent;
			$temp=~ /\<shortinfo\>/i;
			$temp=$';
			$temp=~ /\<\/shortinfo\>/i;
			$attribshortinfo=$`;

			# required の取得
			$attribrequired="";
			$temp=$attribscontent;
			$temp=~ /\<required\>/i;
			$temp=$';
			$temp=~ /\<\/required\>/i;
			$attribrequired=$`;

			# format の取得
			$attribformat="";
			$temp=$attribscontent;
			$temp=~ /\<format\>/i;
			$temp=$';
			$temp=~ /\<\/format\>/i;
			$attribformat=$`;

			# info の取得
			$attribinfo="";
			$temp=$attribscontent;
			$temp=~ /\<info\>/i;
			$temp=$';
			$temp=~ /\<\/info\>/i;
			$attribinfo=$`;

	#		print "shortinfo:",$attribshortinfo,"\n";
	#		print "required:",$attribrequired,"\n";
	#		print "info:",$attribinfo,"\n";

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


sub conv_tdb
{
	$data=$_[0];
	$data=~ s/\n//g;
	$data=~ s/\\/\\\\/g;
	$data=~ s/\{/\\\{/g;
	$data=~ s/\}/\\\}/g;
	$data=~ s/\<TT\>/\{\\f3 /gi;
	$data=~ s/\<\/TT\>/\}/gi;
	$data=~ s/\<BR\>/\\par /gi;
	$data=~ s/\<ref\s+tag\=[\"\']([^\"\']+)[\"\']\>//gi;
	$data=~ s/\<\/ref\>//gi;
	return $data;
}


@h_tagdata=%tagdata;

@outdata=();

for($i=0;$i<=$#h_tagdata;$i+=2)
{
	$od="*".$h_tagdata[$i]."\n";

	$od.=".shortinfo\n";
	$od.=" ".$h_tagdata[$i+1]{"shortinfo"}."\n";

	$od.=".remarks\n";
	$od.=" \{\\b ".$h_tagdata[$i]."タグ\} ( ".&conv_tdb($h_tagdata[$i+1]{"shortinfo"})." )\\par \\par ".&conv_tdb($h_tagdata[$i+1]{"remarks"});
	if($h_tagdata[$i+1]{"example"} ne "")
	{
		$od.="\\par 例:\\par \{\\f3 ".&conv_tdb($h_tagdata[$i+1]{"example"})."\}\n";
	}
	else
	{
		$od.="\n";
	}

	if($h_tagdata[$i+1]{"attribs_data_0"} ne "")
	{


		@h_data=%data;

		$no=0;
		
		while(1)
		{
		
			%data=split(/__SPLIT__/,$h_tagdata[$i+1]{"attribs_data_".$no});
			
			$od.=".attrib:".$data{"nam__e"}."\n";
			$od.="+shortinfo\n";
			$od.=" ".$data{"shortinfo"}."\n";
			$od.="+required\n";
			$od.=" ".$data{"required"}."\n";
			$od.="+format\n";
			$od.=" ".$data{"format"}."\n";
			$od.="+info\n";
			$od.=" \{\\b ".$data{"nam__e"}."属性\} ( ".&conv_tdb($data{"shortinfo"})." )\\par ";
			$od.=&conv_tdb("必須?")." : \{\\b ".&conv_tdb($data{"required"})."\} \\par ";
			$od.="値 : \{\\b ".&conv_tdb($data{"format"})."\} \\par \\par";
			$od.=&conv_tdb($data{"info"})."\n";
			
			$no++;
			
			last if($h_tagdata[$i+1]{"attribs_data_".$no} eq "");
		}
	}

	$od.="\n";

	push(@outdata,$od);
}

foreach $data (sort @outdata)
{
	print $data;
}


