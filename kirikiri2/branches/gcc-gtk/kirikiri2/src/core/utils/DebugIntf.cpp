//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Utilities for Debugging
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include <deque>
#ifdef WIN32
# include <time>
#else
# include <time.h>
#endif
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "StorageIntf.h"
#include "SysInitIntf.h"


//---------------------------------------------------------------------------
// global variables
//---------------------------------------------------------------------------
struct tTVPLogItem
{
	ttstr Log;
	ttstr Time;
	tTVPLogItem(const ttstr &log, const ttstr &time)
	{
		Log = log;
		Time = time;
	}
};
static std::deque<tTVPLogItem> *TVPLogDeque = NULL;
static tjs_uint TVPLogMaxLines = 2048;

static bool TVPAutoLogToFileOnError = true;
static bool TVPAutoClearLogOnError = false;
static bool TVPLoggingToFile = false;
static tjs_uint TVPLogToFileRollBack = 100;
static ttstr *TVPImportantLogs = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static bool TVPLogObjectsInitialized = false;
static void TVPEnsureLogObjects()
{
	if(TVPLogObjectsInitialized) return;
	TVPLogObjectsInitialized = true;

	TVPLogDeque = new std::deque<tTVPLogItem>();
	TVPImportantLogs = new ttstr();
}
//---------------------------------------------------------------------------
static void TVPDestroyLogObjects()
{
	if(TVPLogDeque) delete TVPLogDeque, TVPLogDeque = NULL;
	if(TVPImportantLogs) delete TVPImportantLogs, TVPImportantLogs = NULL;
}
//---------------------------------------------------------------------------
tTVPAtExit TVPDestroyLogObjectsAtExit
	(TVP_ATEXIT_PRI_CLEANUP, TVPDestroyLogObjects);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void (*TVPOnLog)(const ttstr & line) = NULL;
	// this function is invoked when a line is logged
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPSetOnLog
//---------------------------------------------------------------------------
void TVPSetOnLog(void (*func)(const ttstr & line))
{
	TVPOnLog = func;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// log stream holder
//---------------------------------------------------------------------------
class tTVPLogStreamHolder
{
	FILE * Stream;
	bool Alive;
	bool OpenFailed;

public:
	tTVPLogStreamHolder() { Stream = NULL; Alive = true; OpenFailed = false; }
	~tTVPLogStreamHolder() { if(Stream) fclose(Stream); Alive = false; }

private:
	void Open(const tjs_nchar *mode);

public:
	void Clear(); // clear log stream
	void Log(const ttstr & text); // log given text

} static TVPLogStreamHolder;
//---------------------------------------------------------------------------
void tTVPLogStreamHolder::Open(const tjs_nchar *mode)
{
	if(OpenFailed) return; // no more try

	try
	{
		tjs_nchar filename[FILENAME_MAX];
		TJS_nstrcpy(filename, "");
		TJS_nstrcat(filename, TJS_N(".console.log"));
		Stream = fopen(filename, mode);
		if(!Stream) OpenFailed = true;

		if(Stream)
		{
			fseek(Stream, 0, SEEK_END);
			if(ftell(Stream) == 0)
			{
				// write BOM
				// TODO: 32-bit unicode support
				fwrite(TJS_N("\xff\xfe"), 1, 2, Stream); // indicate unicode text
			}

#ifdef TJS_TEXT_OUT_CRLF
			ttstr separator(TJS_W("\r\n\r\n\r\n"
"==============================================================================\r\n"
"==============================================================================\r\n"
				));
#else
			ttstr separator(TJS_W("\n\n\n"
"==============================================================================\n"
"==============================================================================\n"
				));
#endif
			Log(separator);
		}
	}
	catch(...)
	{
		OpenFailed = true;
	}
}
//---------------------------------------------------------------------------
void tTVPLogStreamHolder::Clear()
{
	// clear log text
	if(Stream) fclose(Stream);

	Open(TJS_N("wb"));
}
//---------------------------------------------------------------------------
void tTVPLogStreamHolder::Log(const ttstr & text)
{
	if(!Stream) Open(TJS_N("ab"));

	try
	{
		if(Stream)
		{
			size_t len = text.GetLen() * sizeof(tjs_char);
			if(len != fwrite(text.c_str(), 1, len, Stream))
			{
				// cannot write
				fclose(Stream);
				OpenFailed = true;
				return;
			}
	#ifdef TJS_TEXT_OUT_CRLF
			fwrite(TJS_W("\r\n"), 1, 2 * sizeof(tjs_char), Stream);
	#else
			fwrite(TJS_W("\n"), 1, 1 * sizeof(tjs_char), Stream);
	#endif

			// flush
			fflush(Stream);
		}
	}
	catch(...)
	{
		try
		{
			if(Stream) fclose(Stream);
		}
		catch(...)
		{
		}

		OpenFailed = true;
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// TVPAddLog
//---------------------------------------------------------------------------
void TVPAddLog(const ttstr &line, bool appendtoimportant)
{
	// add a line to the log.
	// exceeded lines over TVPLogMaxLines are eliminated.
	// this function is not thread-safe ...

	TVPEnsureLogObjects();
	if(!TVPLogDeque) return; // log system is shuttingdown
	if(!TVPImportantLogs) return; // log system is shuttingdown

	static time_t prevlogtime = 0;
	static ttstr prevtimebuf;
	static tjs_char timebuf[40];

	tm *struct_tm;
	time_t timer;
	timer = time(&timer);

	if(prevlogtime != timer)
	{
		struct_tm = localtime(&timer);
		TJS_strftime(timebuf, 39, TJS_W("%H:%M:%S"), struct_tm);
		prevlogtime = timer;
		prevtimebuf = timebuf;
	}

	TVPLogDeque->push_back(tTVPLogItem(line, prevtimebuf));

	if(appendtoimportant)
	{
#ifdef TJS_TEXT_OUT_CRLF
		*TVPImportantLogs += ttstr(timebuf) + TJS_W(" ! ") + line + TJS_W("\r\n");
#else
		*TVPImportantLogs += ttstr(timebuf) + TJS_W(" ! ") + line + TJS_W("\n");
#endif
	}
	while(TVPLogDeque->size() >= TVPLogMaxLines+100)
	{
		std::deque<tTVPLogItem>::iterator i = TVPLogDeque->begin();
		TVPLogDeque->erase(i, i+100);
	}

	tjs_int timebuflen = TJS_strlen(timebuf);
	ttstr buf((tTJSStringBufferLength)(timebuflen + 1 + line.GetLen()));
	tjs_char * p = buf.Independ();
	TJS_strcpy(p, timebuf);
	p += timebuflen;
	*p = TJS_W(' ');
	p++;
	TJS_strcpy(p, line.c_str());
	if(TVPOnLog) TVPOnLog(buf);
	if(TVPLoggingToFile) TVPLogStreamHolder.Log(buf);
}
//---------------------------------------------------------------------------
void TVPAddLog(const ttstr &line)
{
	TVPAddLog(line, false);
}
//---------------------------------------------------------------------------
void TVPAddImportantLog(const ttstr &line)
{
	TVPAddLog(line, true);
}
//---------------------------------------------------------------------------
ttstr TVPGetImportantLog()
{
	if(!TVPImportantLogs) return ttstr();
	return *TVPImportantLogs;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPGetLastLog : get last n lines of the log ( each line is spearated with '\n'/'\r\n' )
//---------------------------------------------------------------------------
ttstr TVPGetLastLog(tjs_uint n)
{
	TVPEnsureLogObjects();
	if(!TVPLogDeque) return TJS_W(""); // log system is shuttingdown

	tjs_uint len = 0;
	tjs_uint size = TVPLogDeque->size();
	if(n > size) n = size;
	if(n==0) return ttstr();
	std::deque<tTVPLogItem>::iterator i = TVPLogDeque->end();
	i-=n;
	tjs_uint c;
	for(c = 0; c<n; c++, i++)
	{
#ifdef TJS_TEXT_OUT_CRLF
		len += i->Time.GetLen() + 1 +  i->Log.GetLen() + 2;
#else
		len += i->Time.GetLen() + 1 +  i->Log.GetLen() + 1;
#endif
	}

	ttstr buf((tTJSStringBufferLength)len);
	tjs_char *p = buf.Independ();

	i = TVPLogDeque->end();
	i -= n;
	for(c = 0; c<n; c++)
	{
		TJS_strcpy(p, i->Time.c_str());
		p += i->Time.GetLen();
		*p = TJS_W(' ');
		p++;
		TJS_strcpy(p, i->Log.c_str());
		p += i->Log.GetLen();
#ifdef TJS_TEXT_OUT_CRLF
		*p = TJS_W('\r');
		p++;
		*p = TJS_W('\n');
		p++;
#else
		*p = TJS_W('\n');
		p++;
#endif
		i++;
	}
	return buf;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPStartLogToFile
//---------------------------------------------------------------------------
void TVPStartLogToFile(bool clear)
{
	TVPEnsureLogObjects();
	if(!TVPImportantLogs) return; // log system is shuttingdown

	if(TVPLoggingToFile) return; // already logging
	if(clear) TVPLogStreamHolder.Clear();

	// log last lines

	TVPLogStreamHolder.Log(*TVPImportantLogs);

#ifdef TJS_TEXT_OUT_CRLF
	ttstr separator(TJS_W("\r\n"
"------------------------------------------------------------------------------\r\n"
				));
#else
	ttstr separator(TJS_W("\n"
"------------------------------------------------------------------------------\n"
				));
#endif

	TVPLogStreamHolder.Log(separator);

	ttstr content = TVPGetLastLog(TVPLogToFileRollBack);
	TVPLogStreamHolder.Log(content);

	//
	TVPLoggingToFile = true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPOnError
//---------------------------------------------------------------------------
void TVPOnError()
{
	if(TVPAutoLogToFileOnError) TVPStartLogToFile(TVPAutoClearLogOnError);
	TVPOnErrorHook();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTJSNC_Debug
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_Debug::ClassID = -1;
tTJSNC_Debug::tTJSNC_Debug() : tTJSNativeClass(TJS_W("Debug"))
{
	TJS_BEGIN_NATIVE_MEMBERS(Debug)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/Debug)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Debug)
//----------------------------------------------------------------------

//-- methods

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/message)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(numparams == 1)
	{
		TVPAddLog(*param[0]);
	}
	else
	{
		// display the arguments separated with ", "
		ttstr args;
		for(int i = 0; i<numparams; i++)
		{
			if(i != 0) args += TJS_W(", ");
			args += ttstr(*param[i]);
		}
		TVPAddLog(args);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/message)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/notice)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(numparams == 1)
	{
		TVPAddImportantLog(*param[0]);
	}
	else
	{
		// display the arguments separated with ", "
		ttstr args;
		for(int i = 0; i<numparams; i++)
		{
			if(i != 0) args += TJS_W(", ");
			args += ttstr(*param[i]);
		}
		TVPAddImportantLog(args);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/notice)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/startLogToFile)
{
	bool clear = false;

	if(numparams >= 1)
		clear = *param[0];

	TVPStartLogToFile(clear);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/startLogToFile)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/logAsError)
{
	TVPOnError();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/logAsError)
//----------------------------------------------------------------------

//-- properies

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(logToFileOnError)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPAutoLogToFileOnError;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TVPAutoLogToFileOnError = param->operator bool();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(logToFileOnError)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(clearLogFileOnError)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPAutoClearLogOnError;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TVPAutoClearLogOnError = param->operator bool();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(clearLogFileOnError)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS

	// put version information to DMS
	TVPAddImportantLog(TVPGetVersionInformation());
	TVPAddImportantLog(ttstr(TVPVersionInformation2));

	// check force logging option
	tTJSVariant val;
	if(TVPGetCommandLine(TJS_W("-forcelog"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("yes"))
		{
			TVPStartLogToFile(false);
		}
		else if(str == TJS_W("clear"))
		{
			TVPStartLogToFile(true);
		}
	}
	if(TVPGetCommandLine(TJS_W("-logerror"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("no"))
		{
			TVPAutoClearLogOnError = false;
			TVPAutoLogToFileOnError = false;
		}
		else if(str == TJS_W("clear"))
		{
			TVPAutoClearLogOnError = true;
			TVPAutoLogToFileOnError = true;
		}
	}

} // end of tTJSNC_Debug::tTJSNC_Debug
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TJS2 Console Output Gateway
//---------------------------------------------------------------------------
class tTVPTJS2ConsoleOutputGateway : public iTJSConsoleOutput
{
	void ExceptionPrint(const tjs_char *msg)
	{
		TVPAddLog(msg);
	}

	void Print(const tjs_char *msg)
	{
		TVPAddLog(msg);
	}
} static TVPTJS2ConsoleOutputGateway;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJS2 Dump Output Gateway
//---------------------------------------------------------------------------
static ttstr TVPDumpOutFileName;
static FILE *TVPDumpOutFile = NULL; // use traditional output routine
//---------------------------------------------------------------------------
class tTVPTJS2DumpOutputGateway : public iTJSConsoleOutput
{
	void ExceptionPrint(const tjs_char *msg) { Print(msg); }

	void Print(const tjs_char *msg)
	{
		if(TVPDumpOutFile)
		{
			fwrite(msg, 1, TJS_strlen(msg)*sizeof(tjs_char), TVPDumpOutFile);
#ifdef TJS_TEXT_OUT_CRLF
			fwrite(TJS_W("\r\n"), 1, 2 * sizeof(tjs_char), TVPDumpOutFile);
#else
			fwrite(TJS_W("\n"), 1, 1 * sizeof(tjs_char), TVPDumpOutFile);
#endif
		}
	}
} static TVPTJS2DumpOutputGateway;
//---------------------------------------------------------------------------
void TVPTJS2StartDump()
{
	tjs_nchar filename[FILENAME_MAX];
	TJS_nstrcpy(filename, "");
	TJS_nstrcat(filename, TJS_N(".dump.txt"));
	TVPDumpOutFileName = filename;
	TVPDumpOutFile = fopen(filename, TJS_N("wb+"));
	if(TVPDumpOutFile)
	{
		// TODO: 32-bit unicode support
		fwrite(TJS_N("\xff\xfe"), 1, 2, TVPDumpOutFile); // indicate unicode text
	}
}
//---------------------------------------------------------------------------
void TVPTJS2EndDump()
{
	if(TVPDumpOutFile)
	{
		fclose(TVPDumpOutFile), TVPDumpOutFile = NULL;
		TVPAddLog(ttstr(TJS_W("Dumped to ")) + TVPDumpOutFileName);
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// console interface retrieving functions
//---------------------------------------------------------------------------
iTJSConsoleOutput *TVPGetTJS2ConsoleOutputGateway()
{
	return & TVPTJS2ConsoleOutputGateway;
}
//---------------------------------------------------------------------------
iTJSConsoleOutput *TVPGetTJS2DumpOutputGateway()
{
	return & TVPTJS2DumpOutputGateway;
}
//---------------------------------------------------------------------------


