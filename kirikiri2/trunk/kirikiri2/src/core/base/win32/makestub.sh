#!/bin/sh

convert_crlf()
{
	sed -e 's/[\r\n]+$//' -e 's/$/\r/' $1 > _makestub_temp
	mv _makestub_temp $1
}

perl makestub.pl
convert_crlf "FuncStubs.~cpp"
convert_crlf "FuncStubs.~h"
convert_crlf "../../../plugins/win32/tp_stub.cpp"
convert_crlf "../../../plugins/win32/tp_stub.h"

perl copy_if_differ.pl FuncStubs.~cpp FuncStubs.cpp
perl copy_if_differ.pl FuncStubs.~h FuncStubs.h
