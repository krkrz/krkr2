#include <stdio.h>
#include <locale.h>

#include <tjs.h>
#include <tjsError.h>
#include <tjsNative.h>
#include <tjsString.h>
#include <tjsVariant.h>


#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

using namespace std ;
/*
ios::in     "r"
ios::out    "w"
ios::out | ios::trunc    "w"
ios::out | ios::app      "a"
ios::out | ios::binary   "wb"
ios::out | ios::trunc | ios::binary    "wb"
ios::out | ios::app | ios::binary      "ab"
ios::in | ios::binary                  "rb"
ios::in | ios::out                     "r+"
ios::in | ios::out | ios::trunc        "w+"
ios::in | ios::out | ios::app          "a+"
ios::in | ios::out | ios::binary       "r+b"
ios::in | ios::out | ios::trunc | ios::binary  "w+b"
ios::in | ios::out | ios::app | ios::binary    "a+b"
*/

int GetSize ( std::fstream &strm )
{
	/*
	ios_base::beg : SEEK_SET
	ios_base::end : SEEK_END
	ios_base::cur : SEEK_CUR
	*/
	std::ofstream::pos_type back = strm.tellp () ;
	strm.seekp ( 0, std::ios_base::beg ) ;
	std::ofstream::pos_type a = strm.tellp () ;
	strm.seekg ( 0, std::ios_base::end ) ;
	std::ofstream::pos_type b = strm.tellp () ; 
	strm.seekp ( back ) ;
	return b-a ;
}

bool OpenAndWrite (char* fname, void* buf, int len)
{
	std::fstream cFStrm ( fname, std::ios::out ) ;
	if ( cFStrm.fail() )
		return false ;
    cFStrm.write ( (char*)buf, len ) ;
	return true ;
}

char* OpenAndRead (char* fname)
{
    std::fstream cFStrm ( fname, std::ios::in | std::ios::binary ) ;
	if ( cFStrm.fail () )
		return NULL ;
	int size = GetSize ( cFStrm ) ;
	char* buf = new char[size+1] ;
	cFStrm.read ( buf, size ) ;
	buf[size] = NULL ;
    return buf ;
}

class ConsoleOut : public tTJSNativeFunction
{
public :
    tjs_error Process ( tTJSVariant *result, tjs_int numparams,
        tTJSVariant **param, iTJSDispatch2 *objthis )
    {
        if ( numparams <1 ) return TJS_E_BADPARAMCOUNT ;
        TJS::tTJSString str = param[0]->AsString() ;
        std::cout << str.AsStdString () ;
        return TJS_S_OK ;
    }
};

class ConsoleEnd : public tTJSNativeFunction
{
public :
    tjs_error Process ( tTJSVariant *result, tjs_int numparams,
        tTJSVariant **param, iTJSDispatch2 *objthis )
    {
        if ( numparams >0 ) return TJS_E_BADPARAMCOUNT ;
        std::cout << std::endl;
        return TJS_S_OK ;
    }
};

class ConsoleIn : public tTJSNativeFunction
{
public :
    tjs_error Process ( tTJSVariant *result, tjs_int numparams,
        tTJSVariant **param, iTJSDispatch2 *objthis )
    {
        if ( numparams >0 ) return TJS_E_BADPARAMCOUNT ;
        if ( !result ) return TJS_S_OK ;
        std::string str ;
        std::cin >> str ;
        TJS::tTJSString buf = str.c_str () ;
        *result = buf ;
        return TJS_S_OK ;
    }
};

class ExecClass : public tTJSNativeFunction
{
public :
    tjs_error Process ( tTJSVariant *result, tjs_int numparams,
        tTJSVariant **param, iTJSDispatch2 *objthis )
    {
        int i ;
        if ( numparams <1 ) return TJS_E_BADPARAMCOUNT ;
        if ( !result ) return TJS_S_OK ;
        char** argcbuf = new char*[numparams+2] ;
        std::string *argcstr = new std::string [numparams+1] ;
        TJS::tTJSString str = param[0]->AsString() ;
        argcstr[0] = str.AsStdString () ;
        argcbuf[0] = (char*)argcstr[0].c_str () ;
        for ( i=0; i<numparams ; i++ ) {
            str = param[i]->AsString() ;
            argcstr[i+1] = str.AsStdString () ;
            argcbuf[i+1]= (char*)argcstr[i+1].c_str () ;
        }
        for ( i=0; i<numparams+1; i++ ) {
            std::cout << argcbuf[i] << std::endl ;
        }
        argcbuf[numparams+1] = NULL ;
        *result = ::_execv ( argcbuf[0], &argcbuf[1] ) ;
        delete[] argcbuf ;
        delete[] argcstr ;
        return TJS_S_OK ;
    }
};

int main(int argc, char* argv[])
{
    setlocale (LC_ALL,"" ) ;
    char* buf = OpenAndRead ( "main.tjs" ) ;

    tTJS *tjsengine = new tTJS () ;
    iTJSDispatch2* global = tjsengine->GetGlobalNoAddRef () ;

    iTJSDispatch2 * func = new ConsoleOut () ;
    iTJSDispatch2 * func2 = new ConsoleEnd () ;
    iTJSDispatch2 * func3 = new ConsoleIn () ;
    iTJSDispatch2 * func4 = new ExecClass () ;
    tTJSVariant func_var(func) ;
    tTJSVariant func_var2(func2) ;
    tTJSVariant func_var3(func3) ;
    tTJSVariant func_var4(func4) ;
    func->Release () ;
    func2->Release () ;
    func3->Release () ;
    func4->Release () ;

    TJS_THROW_IF_ERROR (
        global->PropSet ( TJS_MEMBERENSURE, TJS_W("cout"), NULL, &func_var, global ) );
    TJS_THROW_IF_ERROR (
        global->PropSet ( TJS_MEMBERENSURE, TJS_W("endl"), NULL, &func_var2, global ) );
    TJS_THROW_IF_ERROR (
        global->PropSet ( TJS_MEMBERENSURE, TJS_W("cin"), NULL, &func_var3, global ) );
    TJS_THROW_IF_ERROR (
        global->PropSet ( TJS_MEMBERENSURE, TJS_W("exec"), NULL, &func_var4, global ) );

    try {
        tTJSVariant result ;
        tjsengine->ExecScript ( buf,
            &result, NULL,
            NULL ) ;
        printf ( "結果 : %d\n", (int)result ) ;
    }
    
    catch ( TJS::eTJSScriptError &e )
    {
        tjs_int l = e.GetSourceLine () ;
        tjs_int pos = e.GetPosition () ;
        TJS::ttstr tr = e.GetEMessage () ;
        std::cout << "エラー : 行 "<< l << "行目 ブロック名 " << tr.AsStdString() << std::endl ;
    }

    catch (...)
    {
        std::cout << "エラーが発生しました\n" << std::endl ;
    }

    tjsengine->Shutdown () ;
    tjsengine->Release () ;

    getchar () ;
    delete buf ;
	return 0;
}
