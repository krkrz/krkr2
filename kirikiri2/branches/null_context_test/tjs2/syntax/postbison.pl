open(FH, $ARGV[0]);


@all = <FH>;


$all = join('', @all);


$all =~ s/(YYSTYPE;)/$1\nYYLEX_PROTO_DECL\n/i;
$all =~ s/NS_TJS_START/namespace TJS \{/;
$all =~ s/\#line (\d+) \"tjs.y\"/\#line $1 \"..\\bison\\tjs.y\"/g;
$all =~ s/\#line (\d+) \"tjs.yy.c\"/\#line $1 \"..\\bison\\tjs.yy.cpp\"/g;
$all =~ s/\#line (\d+) \"tjspp.y\"/\#line $1 \"..\\bison\\tjspp.y\"/g;
$all =~ s/\#line (\d+) \"tjspp.yy.c\"/\#line $1 \"..\\bison\\tjspp.yy.cpp\"/g;
$all =~ s/\#line (\d+) \"\/usr\/share\/bison\.simple\"/\#line $1 \"..\\bison\\bison\.simple\"/g;
;# $all =~ s/\"\, expecting \`\" : \" or \`\"/\"\, expecting \" : \" or \"/g;
;# $all =~ s/strcat\(msg\, \"\'\"\)\;//g;

open(FH,">".$ARGV[0]);
print FH "#include \"tjsCommHead.h\"\n";
print FH "#pragma hdrstop\n";
print FH $all;
print FH "\n}\n";



