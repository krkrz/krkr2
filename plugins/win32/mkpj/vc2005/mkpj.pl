#!/usr/bin/perl
use encoding 'shift-jis';
use open ':encoding(shift-jis)';

use File::Find;
use File::Path;
use Cwd;
use Win32::Guidgen;


#defファイルの中身
$def_text = <<'END_OF_DEFFILE';
EXPORTS
	V2Link
	V2Unlink
END_OF_DEFFILE

# ファイルチェック
if( !(-e "./project.ini") ) { die "Not found project.ini\n"; }
if( !(-e "./project.pji") ) { die "Not found project.pji\n"; }

# Clean
# VC関係のファイルを消してしまう
foreach(glob("*.ncb")){unlink($_);}
foreach(glob("*.sln")){unlink($_);}
foreach(glob("*.suo")){unlink($_);}
foreach(glob("*.vcproj")){unlink($_);}
foreach(glob("*.user")){unlink($_);}

# INI ファイル読み込み
%params;
print "read ini\n";
open(IN,"<project.ini") or die "Error! : ".$!;
while(<IN>) {
	chomp;
	if( /^([0-9_A-Za-z]+)[ 	]*=[ 	]*(.*)[ 	]*$/ ) {
		$params{$1} = $2;
	}
}
close(IN);


# INI ファイルから読み込んだ値の列挙とチェック
foreach $key ( keys %params ) {
	print "name:$key : value:$params{$key}", "\n";
}
if( !defined($params{'Name'}) ) {
	die "Error! : undefined Name value in ini file.\n";
}

#defファイルの書き出し
$def_file_name = $params{'Name'} . ".def";
open(OUT,">$def_file_name") or die "Error! : ".$!;
print OUT $def_text;
close( OUT );

# ソース、ヘッダー、リソースを実行ディレクトリ以下から検索する
@src_files = ();
@header_files = ();
@resource_files = ();
find(\&list_src_files,'.');
find(\&list_header_files,'.');
find(\&list_resource_files,'.');
sub list_src_files {
	if( -f $_ and $_ =~ /(cpp$)|(.c$)|(.cxx$)|(.rc$)|(.def$)|(.r$)|(.odl$)|(.idl$)|(.hpj$)|(.bat)/ ) {
		push( @src_files, $File::Find::name );
	}
}
sub list_header_files {
	if( -f $_ and $_ =~ /(.h$)|(.hpp$)|(.hxx$)|(.hm$)|(.inl$)/ ) {
		push( @header_files, $File::Find::name );
	}
}
sub list_resource_files {
	if( -f $_ and $_ =~ /(.ico$)|(.cur$)|(.bmp$)|(.dlg$)|(.rc2$)|(.rct$)|(.bin$)|(.rgs$)|(.gif$)|(.jpg$)|(.jpeg$)|(.jpe)/ ) {
		push( @resource_files, $File::Find::name );
	}
}

# 相対パス化
$cwd = cwd();
@rel_src_files = ();
@rel_header_files = ();
@rel_resource_files = ();
foreach( @src_files ) {
	s/$cwd/\./;
	push( @rel_src_files, $_ );
}
foreach( @header_files ) {
	s/$cwd/\./;
	push( @rel_header_files, $_ );
}
foreach( @resource_files ) {
	s/$cwd/\./;
	push( @rel_resource_files, $_ );
}

# ヘッダーのパスからインクルードパスを抽出
%include_path;
foreach( @header_files ) {
	s/$cwd\///;
	s/[^\/]+$//;
	s/\/$//;
	s/\//\\/g;
	if( $_ ne "" ) {
		$include_path{$_} = 1;
	}
}

# インクルードパスを文字列に
$include_path_str="";
foreach $key ( keys( %include_path ) ) {
	$include_path_str = $include_path_str . $key . ";";
}

# ソースファイルなどを文字列に
$src_files_str = "";
$header_files_str = "";
$resource_files_str = "";
foreach( @rel_src_files ) {
	s/\//\\/g;
	$src_files_str = $src_files_str . "\n\t\t\t<File RelativePath=\"" .$_. "\" ></File>";
}
foreach( @rel_header_files ) {
	s/\//\\/g;
	$header_files_str = $header_files_str . "\n\t\t\t<File RelativePath=\"" .$_. "\" ></File>";
}
foreach( @rel_resource_files ) {
	s/\//\\/g;
	$resource_files_str = $resource_files_str . "\n\t\t\t<File RelativePath=\"" .$_. "\" ></File>";
}

#vcprojファイルの書き出し
$project_guid = Win32::Guidgen::create();
$PROJECT_NAME = uc $params{'Name'};
$ProjectName = $params{'Name'};
$vcproj_file_name = $params{'Name'} . ".vcproj";
open(OUT,">$vcproj_file_name") or die "Error! : ".$!;
open(IN,"<project.pji") or die "Error! : ".$!;
while(<IN>) {
	chomp;
	s/%ProjectName%/$ProjectName/;
	s/%PROJECT_GUID%/$project_guid/;
	s/%PROJECT_NAME%/$PROJECT_NAME/;
	s/%HEADER_INCLUDE_DIRS%/$include_path_str/;
	s/%SOURCE_FILES%/$src_files_str/;
	s/%HEADER_FILES%/$header_files_str/;
	s/%RESOURCE_FILES%/$resource_files_str/;
	print OUT $_."\n";
}
close(IN);
close( OUT );



