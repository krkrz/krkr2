//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// KAG Parser Utility Class
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop


#include "KAGParser.h"
#include "StorageIntf.h"
#include "tjsDictionary.h"
#include "MsgIntf.h"
#include "DebugIntf.h"
#include "ScriptMgnIntf.h"
#include "tjsHashSearch.h"
#include "TextStream.h"
#include "tjsGlobalStringMap.h"

//---------------------------------------------------------------------------
/*
  KAG System (Kirikiri Adventure Game System) is an application script for
  TVP(Kirikiri), providing core system for Adventure Game/Visual Novel.
  KAG has a simple tag-based mark-up language ("scenario file").
  Version under 2.x of KAG is slow since the parser is written in TJS1.
  KAG 3.x uses TVP's internal KAG parser written in C++ in this unit, will
  acquire speed in compensation for ability of customizing.
*/
//---------------------------------------------------------------------------


// KAGParser is implemented as a TJS native class/object


//---------------------------------------------------------------------------
// tTJSNI_KAGParser : KAGParser TJS native instance
//---------------------------------------------------------------------------
tTJSNI_KAGParser::tTJSNI_KAGParser()
{
	Owner = NULL;
	Lines = NULL;
	LineCount = 0;
	LabelCached = false;
	IgnoreCR = false;
	DicClear = NULL;
	DicAssign = NULL;
	DicObj = NULL;
	Macros = NULL;
	RecordingMacro = NULL;
	DebugLevel = tkdlSimple;
	Interrupted = false;
	MacroArgStackDepth = 0;
	MacroArgStackBase = 0;

	// retrieve DictClear method and DictObj object
	iTJSDispatch2 * dictclass;
	DicObj = TJSCreateDictionaryObject(&dictclass);
	Macros = TJSCreateDictionaryObject();
	try
	{
		// retrieve clear method from dictclass
		tTJSVariant val;
		tjs_error er;

		er = dictclass->PropGet(0, TJS_W("clear"), NULL, &val, dictclass);
		if(TJS_FAILED(er)) TVPThrowInternalError;
		DicClear = val.AsObject();

		er = dictclass->PropGet(0, TJS_W("assign"), NULL, &val, dictclass);
		if(TJS_FAILED(er)) TVPThrowInternalError;
		DicAssign = val.AsObject();

	}
	catch(...)
	{
		dictclass->Release();
		DicObj->Release();
		Macros->Release();
		if(DicClear) DicClear->Release();
		if(DicAssign) DicAssign->Release();
		throw;
	}

	dictclass->Release();
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
		tTJSNI_KAGParser::Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	Owner = tjs_obj;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_KAGParser::Invalidate()
{
	// invalidate this object

	// release objects
	if(DicAssign) DicAssign->Release();
	if(DicClear) DicClear->Release();
	if(DicObj) DicObj->Release();
	if(Macros) Macros->Release();

	ClearMacroArgs();
	ClearBuffer();

	Owner = NULL;

	inherited::Invalidate();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::operator = (const tTJSNI_KAGParser & ref)
{
	// copy Macros
	{
		tTJSVariant src(ref.Macros, ref.Macros);
		tTJSVariant *psrc = &src;
		DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, Macros);
	}

	// copy MacroArgs
	{
		ClearMacroArgs();

		for(tjs_uint i = 0; i < ref.MacroArgStackDepth; i++)
		{
			iTJSDispatch2 * dic = TJSCreateDictionaryObject();
			iTJSDispatch2 * isrc = ref.MacroArgs[i];
			tTJSVariant src(isrc, isrc);
			tTJSVariant *psrc = &src;
			DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, dic);
			MacroArgs.push_back(dic);
		}
		MacroArgStackDepth = ref.MacroArgStackDepth;
	}
	MacroArgStackBase = ref.MacroArgStackBase;

	// copy CallStack
	CallStack = ref.CallStack;

	// copy StorageName, StorageShortName, Buffer
	StorageName = ref.StorageName;
	StorageShortName = ref.StorageShortName;
	Buffer = ref.Buffer;

	// copy Lines and LineCount
	if(Lines) delete [] Lines;
	Lines = new tLine[ref.LineCount];
	LineCount = ref.LineCount;
	for(tjs_int i = 0; i<LineCount; i++)
	{
		Lines[i].Length = ref.Lines[i].Length;
		Lines[i].Start = (const tjs_char *)Buffer +
			(ref.Lines[i].Start - (const tjs_char *)ref.Buffer);
	}

	// copy CurStorage, CurLine, CurPos
	CurLine = ref.CurLine;
	CurPos = ref.CurPos;

	// copy CurLineStr, LineBuffer, LineBufferUsing
	CurLineStr = ref.CurLineStr;
	LineBuffer = ref.LineBuffer;
	LineBufferUsing = ref.LineBufferUsing;

	// copy CurLabel, CurPage, TagLine
	CurLabel = ref.CurLabel;
	CurPage = ref.CurPage;
	TagLine = ref.TagLine;

	// copy DebugLebel, IgnoreCR
	DebugLevel = ref.DebugLevel;
	IgnoreCR = ref.IgnoreCR;

	// copy RecordingMacro, RecordingMacroStr, RecordingMacroName
	RecordingMacro = ref.RecordingMacro;
	RecordingMacroStr = ref.RecordingMacroStr;
	RecordingMacroName = ref.RecordingMacroName;

	// copy ExcludeLevel, IfLevel
	ExcludeLevel = ref.ExcludeLevel;
	IfLevel = ref.IfLevel;
}
//---------------------------------------------------------------------------
iTJSDispatch2 *tTJSNI_KAGParser::Store()
{
	// store current status into newly created dictionary object
	// and return the dictionary object.
	iTJSDispatch2 * dic = TJSCreateDictionaryObject();
	try
	{
		tTJSVariant val;

		// create and assign macro dictionary
		{
			iTJSDispatch2 * dsp;

			dsp = TJSCreateDictionaryObject();
			tTJSVariant tmp(dsp, dsp);
			dsp->Release();
			dic->PropSet(TJS_MEMBERENSURE, TJS_W("macros"), NULL,
				&tmp, dic);

			tTJSVariant src(Macros, Macros);
			tTJSVariant *psrc = &src;
			DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, dsp);
		}

		// create and assign macro arguments
		{
			iTJSDispatch2 *dsp;
			dsp = TJSCreateArrayObject();
			tTJSVariant tmp(dsp, dsp);
			dsp->Release();
			dic->PropSet(TJS_MEMBERENSURE, TJS_W("macroArgs"), NULL,
				&tmp, dic);

			for(tjs_uint i = 0; i < MacroArgStackDepth; i++)
			{
				iTJSDispatch2 *dic;
				dic = TJSCreateDictionaryObject();
				tTJSVariant tmp(dic, dic);
				dic->Release();
				dsp->PropSetByNum(TJS_MEMBERENSURE, i,
					&tmp, dsp);

				iTJSDispatch2 * isrc = MacroArgs[i];
				tTJSVariant src(isrc, isrc);
				tTJSVariant *psrc = &src;
				DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, dic);
			}
		}


		// create call stack array and copy call stack status
		{
			iTJSDispatch2 *dsp;
			dsp = TJSCreateArrayObject();
			tTJSVariant tmp(dsp, dsp);
			dsp->Release();
			dic->PropSet(TJS_MEMBERENSURE, TJS_W("callStack"), NULL,
				&tmp, dic);

			std::vector<tCallStackData>::iterator i;
			for(i = CallStack.begin(); i != CallStack.end(); i++)
			{
				iTJSDispatch2 *dic;
				dic = TJSCreateDictionaryObject();
				tTJSVariant tmp(dic, dic);
				dic->Release();
				dsp->PropSetByNum(TJS_MEMBERENSURE, i - CallStack.begin(),
					&tmp, dsp);

				tTJSVariant val;
				val = i->Storage;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("storage"), NULL,
					&val, dic);
				val = i->Label;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("label"), NULL,
					&val, dic);
				val = i->Offset;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("offset"), NULL,
					&val, dic);
				val = i->OrgLineStr;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("orgLineStr"), NULL,
					&val, dic);
				val = i->LineBuffer;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("lineBuffer"), NULL,
					&val, dic);
				val = i->Pos;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("pos"), NULL,
					&val, dic);
				val = (tjs_int)i->LineBufferUsing;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("lineBufferUsing"), NULL,
					&val, dic);
				val = (tjs_int)i->MacroArgStackBase;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("macroArgStackBase"), NULL,
					&val, dic);
				val = (tjs_int)i->MacroArgStackDepth;
				dic->PropSet(TJS_MEMBERENSURE, TJS_W("macroArgStackDepth"), NULL,
					&val, dic);
			}
		}
		
		// store StorageName, StorageShortName ( Buffer is not stored )
		val = StorageName;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("storageName"), NULL,
			&val, dic);
		val = StorageShortName;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("storageShortName"), NULL,
			&val, dic);

		// ( Lines and LineCount are not stored )

		// store CurStorage, CurLine, CurPos
		val = CurLine;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("curLine"), NULL,
			&val, dic);
		val = CurPos;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("curPos"), NULL,
			&val, dic);

		// ( CurLineStr is not stored )

		// LineBuffer, LineBufferUsing
		val = LineBuffer;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("lineBuffer"), NULL,
			&val, dic);
		val = (tjs_int)LineBufferUsing;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("lineBufferUsing"), NULL,
			&val, dic);

		// store CurLabel ( CurPage TagLine is not stored )
		val = CurLabel;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("curLabel"), NULL,
			&val, dic);

		// ( DebugLebel and IgnoreCR are not stored )

		// ( RecordingMacro, RecordingMacroStr, RecordingMacroName are not stored)

		// ( ExcludeLevel, IfLevel are not stored )

		// store MacroArgStackBase, MacroArgStackDepth
		val = (tjs_int)MacroArgStackBase;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("macroArgStackBase"), NULL,
			&val, dic);

		val = (tjs_int)MacroArgStackDepth;
		dic->PropSet(TJS_MEMBERENSURE, TJS_W("macroArgStackDepth"), NULL,
			&val, dic);

	}
	catch(...)
	{
		dic->Release();
		throw;
	}
	return dic;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::Restore(iTJSDispatch2 *dic)
{
	// restore status from "dic"
	tTJSVariant val;
	tjs_error er;

	// restore macros
	{
		val.Clear();
		dic->PropGet(0, TJS_W("macros"), NULL, &val, dic);
		if(val.Type() != tvtVoid)
		{
			tTJSVariant *psrc = &val;
			DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, Macros);
		}
	}

	{
		// restore macro args
		MacroArgStackDepth = 0;

		val.Clear();
		dic->PropGet(0, TJS_W("macroArgs"), NULL, &val, dic);
		if(val.Type() != tvtVoid)
		{
			tTJSVariantClosure clo = val.AsObjectClosureNoAddRef();
			tTJSVariant v;
			tjs_int count = 0;
			clo.PropGet(0, TJS_W("count"), NULL, &v, NULL);
			count = v;

			ClearMacroArgs();

			val.Clear();
			dic->PropGet(0, TJS_W("macroArgStackDepth"), NULL,
				&val, dic);
			if(val.Type() != tvtVoid) MacroArgStackDepth = (tjs_uint)(tjs_int)val;

			for(tjs_int i = 0; i<count; i++)
			{
				iTJSDispatch2 *dsp = TJSCreateDictionaryObject();
				tTJSVariant val(dsp, dsp);
				dsp->Release();

				clo.PropGetByNum(0, i, &v, NULL);
				tTJSVariant *psrc = &v;
				DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, dsp);

				dsp->AddRef();
				MacroArgs.push_back(dsp);
			}
		}

		if(MacroArgStackDepth != MacroArgs.size())
			TVPThrowExceptionMessage(TVPKAGMalformedSaveData);

		MacroArgStackBase = MacroArgs.size(); // later reset to MacroArgStackBase

		// restore call stack
		val.Clear();
		dic->PropGet(0, TJS_W("callStack"), NULL, &val, dic);
		if(val.Type() != tvtVoid)
		{
			tTJSVariantClosure clo = val.AsObjectClosureNoAddRef();
			tTJSVariant v;
			tjs_int count = 0;
			clo.PropGet(0, TJS_W("count"), NULL, &v, NULL);
			count = v;

			CallStack.clear();

			for(tjs_int i = 0; i < count; i++)
			{
				ttstr Storage;
				ttstr Label;
				tjs_int Offset;
				ttstr OrgLineStr;
				ttstr LineBuffer;
				tjs_int Pos;
				bool LineBufferUsing;
				tjs_uint MacroArgStackBase;
				tjs_uint MacroArgStackDepth;

				tTJSVariant val;

				clo.PropGetByNum(0, i, &v, NULL);
				tTJSVariantClosure dic = v.AsObjectClosureNoAddRef();
				dic.PropGet(0, TJS_W("storage"), NULL, &val, NULL);
				Storage = val;
				dic.PropGet(0, TJS_W("label"), NULL, &val, NULL);
				Label = val;
				dic.PropGet(0, TJS_W("offset"), NULL, &val, NULL);
				Offset = val;
				dic.PropGet(0, TJS_W("orgLineStr"), NULL, &val, NULL);
				OrgLineStr = val;
				dic.PropGet(0, TJS_W("lineBuffer"), NULL, &val, NULL);
				LineBuffer = val;
				dic.PropGet(0, TJS_W("pos"), NULL, &val, NULL);
				Pos = val;
				dic.PropGet(0, TJS_W("lineBufferUsing"), NULL, &val, NULL);
				LineBufferUsing = val;
				dic.PropGet(0, TJS_W("macroArgStackBase"), NULL, &val, NULL);
				MacroArgStackBase = (tjs_int)val;
				dic.PropGet(0, TJS_W("macroArgStackDepth"), NULL, &val, NULL);
				MacroArgStackDepth = (tjs_int)val;

				CallStack.push_back(tCallStackData(
					Storage, Label, Offset, OrgLineStr, LineBuffer, Pos,
						LineBufferUsing, MacroArgStackBase, MacroArgStackDepth));
			}
		}

		// restore StorageName, StorageShortName, CurStorage, CurLabel
		val.Clear();
		dic->PropGet(0, TJS_W("storageName"), NULL,
			&val, dic);
		if(val.Type() != tvtVoid) StorageName = val;
		val.Clear();
		dic->PropGet(0, TJS_W("storageShortName"), NULL,
			&val, dic);
		if(val.Type() != tvtVoid) StorageShortName = val;
		val.Clear();
		dic->PropGet(0, TJS_W("curLabel"), NULL,
			&val, dic);
		if(val.Type() != tvtVoid) CurLabel = val;

		// load scenario
		ttstr storage = StorageName, label = CurLabel;
		ClearBuffer(); // ensure re-loading the scenario
		LoadScenario(storage);
		GoToLabel(label);

		// restore MacroArgStackBase
		val.Clear();
		dic->PropGet(0, TJS_W("macroArgStackBase"), NULL,
			&val, dic);
		if(val.Type() != tvtVoid) MacroArgStackBase = (tjs_uint)(tjs_int)val;

	}
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::LoadScenario(const ttstr & name)
{
	// load scenario to buffer

	BreakConditionAndMacro();


	if(StorageName == name)
	{
		// avoid re-loading
		Rewind();
	}
	else
	{
		ClearBuffer();

		// fire onScenarioLoad
		tTJSVariant param = name;
		tTJSVariant *pparam = &param;
		tTJSVariant result;
		static ttstr funcname(TJSMapGlobalStringMap(TJS_W("onScenarioLoad")));
		tjs_error status = Owner->FuncCall(0, funcname.c_str(), funcname.GetHint(),
			&result, 1, &pparam, Owner);

		if(status == TJS_S_OK && result.Type() == tvtString)
		{
			// when onScenarioLoad returns string;
			// assumes the string is scenario
			Buffer = ttstr(result).c_str();
		}
		else
		{
			// else load from file

			tTJSTextReadStream * stream = NULL;

			try
			{
				stream = TVPCreateTextStreamForRead(name, TJS_W(""));
				ttstr tmp;
				stream->Read(tmp, 0);
				Buffer = tmp.c_str();
				delete stream, stream = NULL;
			}
			catch(...)
			{
				if(stream) delete stream;
				ClearBuffer();
				throw;
			}
		}

		try
		{
			tjs_char *buffer_p = Buffer;

			// count lines
			tjs_int count = 0;
			tjs_char *ls = buffer_p;
			tjs_char *p = buffer_p;
			while(*p)
			{
				if(*p == TJS_W('\r') || *p == TJS_W('\n'))
				{
					count++;
					if(*p == TJS_W('\r') && p[1] == TJS_W('\n')) p++;
					p++;
					ls = p;
				}
				else
				{
					p++;
				}
			}

			if(p!=ls)
			{
				count++;
			}

			if(count == 0) TVPThrowExceptionMessage(TVPKAGNoLine, name);

			Lines = new tLine[count];
			LineCount = count;

			count = 0;
			ls = buffer_p;
			while(*ls == '\t') ls++; // skip leading tabs
			p = ls;
			while(*p)
			{
				if(*p == TJS_W('\r') || *p == TJS_W('\n'))
				{
					Lines[count].Start = ls;
					Lines[count].Length = p-ls;
					count++;
					tjs_char *rp = p;
					if(*p == TJS_W('\r') && p[1] == TJS_W('\n')) p++;
					p++;
					ls = p;
					while(*ls == '\t') ls++;  // skip leading tabs
					p = ls;
					*rp = 0; // end line with null terminater
				}
				else
				{
					p++;
				}
			}

			if(p != ls)
			{
				Lines[count].Start = ls;
				Lines[count].Length = p-ls;
			}

			Rewind();

		}
		catch(...)
		{
			ClearBuffer();
			throw;
		}

		StorageName = name;
		StorageShortName = TVPExtractStorageName(name);

		if(DebugLevel >= tkdlSimple)
		{
			static ttstr hr(TJS_W(
				"========================================"
				"========================================"));
			TVPAddLog(hr);
			TVPAddLog(TJS_W("Scenario loaded : ") + name);
		}
	}


	if(Owner)
	{
		tTJSVariant param = StorageName;
		tTJSVariant *pparam = &param;
		static ttstr funcname(TJSMapGlobalStringMap(TJS_W("onScenarioLoaded")));
		Owner->FuncCall(0, funcname.c_str(), funcname.GetHint(),
			NULL, 1, &pparam, Owner);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::Clear()
{
	// clear all states
	ClearBuffer();
	ClearMacroArgs();
	ClearCallStack();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::ClearBuffer()
{
	// clear internal buffer
	Buffer.Clear();
	if(Lines) delete [] Lines, Lines = NULL;
	LabelCache.Clear();
	LabelAliases.clear();
	LabelCached = false;
	LineCount = 0;
	StorageName.Clear();
	StorageShortName.Clear();
	BreakConditionAndMacro();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::Rewind()
{
	// set current position to first
	CurLine = 0;
	CurPos = 0;
	CurLineStr = Lines[0].Start;
	LineBufferUsing = false;
	BreakConditionAndMacro();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::BreakConditionAndMacro()
{
	// break condition state and macro recording
	RecordingMacro = false;
	ExcludeLevel = -1;
	IfLevel = 0;
	PopMacroArgsTo(MacroArgStackBase);
		// clear macro argument down to current base stack position
}
//---------------------------------------------------------------------------
const ttstr & tTJSNI_KAGParser::GetLabelAliasFromLine(tjs_int line) const
{
	// retrieve label alias at given line
	return LabelAliases[line];
}
//---------------------------------------------------------------------------
static bool inline TVPIsWS(tjs_char ch)
{
	// is white space ?
	return (ch == TJS_W(' ') || ch == TJS_W('\t'));
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::EnsureLabelCache()
{
	// construct label cache
	if(!LabelCached)
	{
		// make label cache
		LabelAliases.resize(LineCount);
		ttstr prevlabel;
		const tjs_char *p;
		const tjs_char *vl;
		tjs_int i;
		for(i = 0; i<LineCount; i++)
		{
			if(Lines[i].Length >= 2 &&
				Lines[i].Start[0] == TJS_W('*'))
			{
				p = Lines[i].Start;
				vl = TJS_strchr(p, TJS_W('|'));
				ttstr label;
				if(vl)
				{
					// page name found
					label = ttstr(p, vl-p);
				}
				else
				{
					label = p;
				}

				if(!label.c_str()[1])
				{
					if(prevlabel.IsEmpty())
						TVPThrowExceptionMessage(TVPKAGCannotOmmitFirstLabelName);
					label = prevlabel;
				}

				prevlabel = label;

				tLabelCacheData *data = LabelCache.Find(label);
				if(data)
				{
					// previous label name found (duplicated label)
					data->Count++;
					label = label + TJS_W(":") + ttstr(data->Count);
				}

				LabelCache.Add(label, tLabelCacheData(i, 1));
				LabelAliases[i] = label;
			}
		}

		LabelCached = true;
	}

}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::GoToLabel(const ttstr &name)
{
	// search label and set current position
	// parameter "name" must start with '*'
	if(name.IsEmpty()) return;

	EnsureLabelCache();

	tLabelCacheData *newline;

	newline = LabelCache.Find(name);

	if(newline)
	{
		// found the label in cache
		const tjs_char *vl;
		vl = TJS_strchr(Lines[newline->Line].Start, TJS_W('|'));

		CurLabel = GetLabelAliasFromLine(newline->Line);
		if(vl) CurPage = vl+1; else CurPage.Clear();
		CurLine = newline->Line;
		CurPos = 0;
		LineBufferUsing = false;
	}
	else
	{
		// not found
		TVPThrowExceptionMessage(TVPKAGLabelNotFound, StorageName, name);
	}

	if(DebugLevel >= tkdlSimple)
	{
		static ttstr hr(TJS_W(
			"- - - - - - - - - - - - - - - - - - - - "
			"- - - - - - - - - - - - - - - - - - - - "));
		TVPAddLog(hr);
		TVPAddLog(StorageShortName + TJS_W(" : jumped to : ") + name);
	}

	BreakConditionAndMacro();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::GoToStorageAndLabel(const ttstr &storage,
	const ttstr &label)
{
	if(!storage.IsEmpty()) LoadScenario(storage);
	if(!label.IsEmpty()) GoToLabel(label);
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::CallLabel(const ttstr &name)
{
	PushCallStack();
	GoToLabel(name);
}
//---------------------------------------------------------------------------
bool tTJSNI_KAGParser::SkipCommentOrLabel()
{
	// skip comment or label, and go to next line.
	// fire OnScript event if [script] thru [endscript] ( or @script thru
	// @endscript ) is found.
	EnsureLabelCache();

	CurPos = 0;
	if(!Lines) return false;
	if(CurLine >= LineCount) return false;
	for(; CurLine < LineCount; CurLine++)
	{
		const tjs_char * p = Lines[CurLine].Start;

		if(p[0] == TJS_W(';'))
			continue; // comment

		if(p[0] == TJS_W('*'))
		{
			// label
			if(RecordingMacro)
				TVPThrowExceptionMessage(TVPLabelOrScriptInMacro);

			tjs_char * vl = TJS_strchr(p, TJS_W('|'));
			bool pagename;
			if(vl)
			{
				CurLabel = GetLabelAliasFromLine(CurLine);
				CurPage = ttstr(vl + 1);
				pagename = true;
			}
			else
			{
				CurLabel = GetLabelAliasFromLine(CurLine);
				CurPage.Clear();
				pagename = false;
			}

			// fire onLabel callback event
			if(Owner)
			{
				tTJSVariant param[2];
				param[0] = CurLabel;
				if(pagename) param[1] = CurPage;
				tTJSVariant *pparam[2] = { param, param+1 };
				static ttstr onLabel_name(TJSMapGlobalStringMap(TJS_W("onLabel")));
				Owner->FuncCall(0, onLabel_name.c_str(), onLabel_name.GetHint(),
					NULL, 2, pparam, Owner);
			}

			continue;
		}

		if(p[0] == TJS_W('[') &&
			(!TJS_strcmp(p, TJS_W("[iscript]")) ||
			 !TJS_strcmp(p, TJS_W("[iscript]\\")) )||
		   p[0] == TJS_W('@') &&
			(!TJS_strcmp(p, TJS_W("@iscript")) )    )
		{
			// inline TJS script
 			if(RecordingMacro)
				TVPThrowExceptionMessage(TVPLabelOrScriptInMacro);

			ttstr script;
			CurLine++;

			tjs_int script_start = CurLine;

			for(;CurLine < LineCount; CurLine++)
			{
				p = Lines[CurLine].Start;
				if(p[0] == TJS_W('[') &&
					(!TJS_strcmp(p, TJS_W("[endscript]")) ||
					 !TJS_strcmp(p, TJS_W("[endscript]\\")) )||
				  p[0] == TJS_W('@') &&
					(!TJS_strcmp(p, TJS_W("@endscript")) )    )
				{
					break;
				}

				if(ExcludeLevel == -1)
				{
					script += p;
					script += TJS_W("\r\n");
				}
			}

			if(CurLine == LineCount)
				 TVPThrowExceptionMessage(TVPKAGInlineScriptNotEnd);

			// fire onScript callback event
			if(ExcludeLevel == -1)
			{
				if(Owner)
				{
					tTJSVariant param[3] = {script, StorageShortName, script_start};
					tTJSVariant *pparam[3] = { param, param+1, param+2 };
					static ttstr onScript_name(TJSMapGlobalStringMap(TJS_W("onScript")));
					Owner->FuncCall(0, onScript_name.c_str(), onScript_name.GetHint(),
						NULL, 3, pparam, Owner);
				}
			}

			continue;
		}

		break;
	}

	if(CurLine >= LineCount) return false;

	CurLineStr = Lines[CurLine].Start;
	LineBufferUsing = false;

	if(DebugLevel >= tkdlVerbose)
	{
		TVPAddLog(StorageShortName + TJS_W(" : ") + CurLineStr);
	}

	return true;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::PushMacroArgs(iTJSDispatch2 *args)
{
	iTJSDispatch2 *dsp;
	if(MacroArgs.size() > MacroArgStackDepth)
	{
		dsp = MacroArgs[MacroArgStackDepth];
	}
	else
	{
		if(MacroArgStackDepth > MacroArgs.size())
			TVPThrowInternalError;
		dsp = TJSCreateDictionaryObject();
		MacroArgs.push_back(dsp);
	}
	MacroArgStackDepth++;

	// copy arguments from args to dsp
	tTJSVariant src(args, args);
	tTJSVariant *psrc = &src;
	DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, dsp);
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::PopMacroArgs()
{
	if(MacroArgStackDepth == 0) TVPThrowExceptionMessage(TVPKAGSyntaxError);
	MacroArgStackDepth--;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::ClearMacroArgs()
{
	for(std::vector<iTJSDispatch2 *>::iterator i = MacroArgs.begin();
		i != MacroArgs.end(); i++)
	{
		(*i)->Release();
	}
	MacroArgs.clear();
	MacroArgStackDepth = 0;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::PopMacroArgsTo(tjs_uint base)
{
	MacroArgStackDepth = base;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::FindNearestLabel(tjs_int start, tjs_int &labelline,
	ttstr &labelname)
{
	// find nearest label which be pleced before "start".
	// "labelline" is to be the label's line number (0-based), and
	// "labelname" is to be its label name.
	// "labelline" will be -1 and "labelname" be empty if the label is not found.
	EnsureLabelCache();

	start--;
	while(start >= 0)
	{
		if(Lines[start].Start[0] == TJS_W('*'))
		{
			// label found
			labelname = GetLabelAliasFromLine(start);
			break;
		}
		start --;
	}
	labelline = start;
	if(labelline == -1) labelname.Clear();
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::PushCallStack()
{
	// push current position information
	if(DebugLevel >= tkdlVerbose)
	{
		TVPAddLog(StorageShortName + TJS_W(" : call stack depth before calling : ")
			+ ttstr((int)CallStack.size()));
	}

	tjs_int labelline;
	ttstr labelname;
	FindNearestLabel(CurLine, labelline, labelname);
	if(labelline < 0) labelline = 0;

	const wchar_t *curline_content;
	if(Lines && CurLine < LineCount)
		curline_content = Lines[CurLine].Start;
	else
		curline_content = TJS_W("");

	CallStack.push_back(tCallStackData(StorageName, labelname, CurLine - labelline,
		curline_content,
		LineBuffer, CurPos, LineBufferUsing, MacroArgStackBase, MacroArgStackDepth));
	MacroArgStackBase = MacroArgStackDepth;
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::PopCallStack(const ttstr &storage, const ttstr &label)
{
	// pop call stack information
	if(CallStack.size() == 0)
		TVPThrowExceptionMessage(TVPKAGCallStackUnderflow);

	// pop macro argument information
	tCallStackData & data = CallStack.back();
	MacroArgStackBase = data.MacroArgStackDepth; // later reset to MacroArgStackBase
	PopMacroArgsTo(data.MacroArgStackDepth);

	// goto label or previous position
	if(!storage.IsEmpty() || !label.IsEmpty())
	{
		// return to specified position
		GoToStorageAndLabel(storage, label);
	}
	else
	{
		// return to previous calling position
		LoadScenario(data.Storage);
		if(!data.Label.IsEmpty()) GoToLabel(data.Label);
		CurLine += data.Offset;
		if(CurLine > LineCount)
			TVPThrowExceptionMessage(TVPKAGReturnLostSync);
				/* CurLine == LineCount is OK (at end of file) */
		if(CurLine < LineCount)
			if(data.OrgLineStr != Lines[CurLine].Start) // check original line information
				TVPThrowExceptionMessage(TVPKAGReturnLostSync);
		if(data.LineBufferUsing)
		{
			LineBuffer = data.LineBuffer;
			CurLineStr = LineBuffer.c_str();
			LineBufferUsing = true;
		}
		else
		{
			if(CurLine < LineCount)
			{
				CurLineStr = Lines[CurLine].Start;
				LineBufferUsing = false;
			}
		}
		CurPos = data.Pos;

		if(DebugLevel >= tkdlSimple)
		{
			ttstr label;
			if(data.Label.IsEmpty()) label = TJS_W("(start)"); else label = data.Label;
			TVPAddLog(StorageShortName + TJS_W(" : returned to : ") + label +
				TJS_W(" line offset ") + ttstr(data.Offset));
		}
	}

	// reset MacroArgStackBase
	MacroArgStackBase = data.MacroArgStackBase;

	// pop casll stack
	CallStack.pop_back();


	// call function back
	if(Owner)
	{
		static ttstr onAfterReturn_name(TJS_W("onAfterReturn"));
		Owner->FuncCall(0, onAfterReturn_name.c_str(), onAfterReturn_name.GetHint(),
			NULL, 0, NULL, Owner);
	}


	if(DebugLevel >= tkdlVerbose)
	{
		TVPAddLog(StorageShortName + TJS_W(" : call stack depth after returning : ")
			+ ttstr((int)CallStack.size()));
	}
}
//---------------------------------------------------------------------------
void tTJSNI_KAGParser::ClearCallStack()
{
	CallStack.clear();
	PopMacroArgsTo(MacroArgStackBase = 0); // macro arguments are also cleared
}
//---------------------------------------------------------------------------
iTJSDispatch2 * tTJSNI_KAGParser::_GetNextTag()
{
	// get next tag and return information dictionary object.
	// return NULL if the tag not found.
	// normal characters are interpreted as a "ch" tag.
	// CR code is interpreted as a "r" tag.
	// returned tag's line number is stored to TagLine.
	// tag paremeters are stored into return value.
	// tag name is also stored into return value, naemd "__tag".

	// pretty a nasty code.
parse_start:

	if(CurLine >= LineCount) return NULL;
	if(!Lines) return NULL;


	static ttstr __tag_name(TJSMapGlobalStringMap(TJS_W("tagname")));
	static ttstr __storage_name(TJSMapGlobalStringMap(TJS_W("storage")));
	static ttstr __target_name(TJSMapGlobalStringMap(TJS_W("target")));
	static ttstr __exp_name(TJSMapGlobalStringMap(TJS_W("exp")));

	while(true)
	{

		DicClear->FuncCall(0, NULL, NULL, NULL, 0, NULL, DicObj);
			// clear dictionary object

		if(Interrupted)
		{
			// interrupt current parsing
			// return as "interrupted" tag
			static tTJSVariant r_val(TJS_W("interrupt"));
			DicObj->PropSetByVS(TJS_MEMBERENSURE,
				__tag_name.AsVariantStringNoAddRef(), &r_val, DicObj);
			Interrupted = false;
			DicObj->AddRef();
			return DicObj;
		}

		if(CurLine >= LineCount) break; // all of scenario was decoded 

		tjs_int tagstartpos = CurPos;

		if(!LineBufferUsing && CurPos == 0)
			if(!SkipCommentOrLabel()) return NULL;

		if(!IgnoreCR)
		{
			if(CurLineStr[CurPos] == TJS_W('\\') &&
				CurLineStr[CurPos+1] == 0  ||
				CurLineStr[CurPos] == 0 && CurPos >= 3 &&
				CurLineStr[CurPos-3] == TJS_W('[') &&
				CurLineStr[CurPos-2] == TJS_W('p') && // for line ending with [p]
				CurLineStr[CurPos-1] == TJS_W(']') )
			{
				// line ended with '\\'
				CurLine++;
				CurPos = 0;
				LineBufferUsing = false;
				continue;
			}

			if(CurLineStr[CurPos] == 0)
			{
				// line ended ...
				TagLine = CurLine;
				static tTJSVariant r_val(TJS_W("r"));
				DicObj->PropSetByVS(TJS_MEMBERENSURE,
					__tag_name.AsVariantStringNoAddRef(), &r_val, DicObj);
				if(RecordingMacro) RecordingMacroStr += TJS_W("[r]");
				CurLine++;
				CurPos = 0;
				LineBufferUsing = false;
				if(!RecordingMacro && ExcludeLevel == -1)
				{
					DicObj->AddRef();
					return DicObj;
				}
				continue;
			}
		}

		tjs_char ldelim; // last delimiter

		if(!LineBufferUsing && CurPos == 0 && CurLineStr[0] == TJS_W('@'))
		{
			// line command mode
			ldelim = 0; // tag last delimiter is a null terminater
		}
		else
		{
			if(CurLineStr[CurPos] != TJS_W('[') ||
				CurLineStr[CurPos] == TJS_W('[') &&
				CurLineStr[CurPos+1] == TJS_W('['))
			{
				// normal character
				tjs_char ch = CurLineStr[CurPos];
				TagLine = CurLine;

				if(ch == 0)
				{
					// line ended
					CurLine++;
					CurPos = 0;
					LineBufferUsing = false;
					continue;
				}
				else if(ch == TJS_W('\t'))
				{
					CurPos++;
					continue;
				}
				else if(ch != TJS_W('\n'))
				{
					static tTJSVariant tag_val(TJS_W("ch"));
					DicObj->PropSetByVS(TJS_MEMBERENSURE,
						__tag_name.AsVariantStringNoAddRef(), &tag_val, DicObj);
					tTJSVariant ch_val(ttstr(CurLineStr + CurPos, 1));
					static ttstr text_name(TJSMapGlobalStringMap(TJS_W("text")));
					DicObj->PropSetByVS(TJS_MEMBERENSURE,
						text_name.AsVariantStringNoAddRef(), &ch_val, DicObj);

					if(RecordingMacro)
					{
						if(ch == TJS_W('['))
							RecordingMacroStr += TJS_W("[[");
						else
							RecordingMacroStr += ch;
					}
				}
				else
				{
					// \n  ( reline )
					static tTJSVariant r_val(TJS_W("r"));
					DicObj->PropSetByVS(TJS_MEMBERENSURE,
						__tag_name.AsVariantStringNoAddRef(), &r_val, DicObj);
					if(RecordingMacro) RecordingMacroStr += TJS_W("[r]");
				}

				if(CurLineStr[CurPos] == TJS_W('[')) CurPos++;
				CurPos++;

				if(!RecordingMacro && ExcludeLevel == -1)
				{
					DicObj->AddRef();
					return DicObj;
				}
				continue;
			}

			ldelim = TJS_W(']');
		}

		// a tag
		bool condition = true;
		TagLine = CurLine;
		tjs_int tagstart = CurPos;
		CurPos ++;

		if(CurLineStr[CurPos] == 0) TVPThrowExceptionMessage(TVPKAGSyntaxError);

		// tag name
		while(TVPIsWS(CurLineStr[CurPos])) CurPos ++;
		if(CurLineStr[CurPos] == 0) TVPThrowExceptionMessage(TVPKAGSyntaxError);
		const tjs_char * tagnamestart = CurLineStr + CurPos;
		while(CurLineStr[CurPos] && !TVPIsWS(CurLineStr[CurPos]) &&
			CurLineStr[CurPos] != ldelim)
				CurPos ++;

		if(tagnamestart == CurLineStr + CurPos)
			TVPThrowExceptionMessage(TVPKAGSyntaxError);

		ttstr tagname(tagnamestart, CurLineStr + CurPos - tagnamestart);
		tagname.ToLowerCase();
		{

			tTJSVariant tag_val(tagname);
			DicObj->PropSetByVS(TJS_MEMBERENSURE,
				__tag_name.AsVariantStringNoAddRef(), &tag_val, DicObj);
		}

		// check special control tags
		enum tSpecialTags
		{ tag_other, tag_if, tag_ignore, tag_endif, tag_endignore,
			tag_emb, tag_macro, tag_endmacro, tag_macropop, tag_erasemacro,
			tag_jump, tag_call, tag_return} tagkind;
		static bool tag_checker_init = false;
		static tTJSHashTable<ttstr, tjs_int> special_tags_hash;
		if(!tag_checker_init)
		{
			tag_checker_init = true;
			special_tags_hash.Add(
				ttstr(TJS_W("if")), (tjs_int)tag_if);
			special_tags_hash.Add(
				ttstr(TJS_W("ignore")), (tjs_int)tag_ignore);
			special_tags_hash.Add(
				ttstr(TJS_W("endif")), (tjs_int)tag_endif);
			special_tags_hash.Add(
				ttstr(TJS_W("endignore")), (tjs_int)tag_endignore);
			special_tags_hash.Add(
				ttstr(TJS_W("emb")), (tjs_int)tag_emb);
			special_tags_hash.Add(
				ttstr(TJS_W("macro")), (tjs_int)tag_macro);
			special_tags_hash.Add(
				ttstr(TJS_W("endmacro")), (tjs_int)tag_endmacro);
			special_tags_hash.Add(
				ttstr(TJS_W("macropop")), (tjs_int)tag_macropop);
			special_tags_hash.Add(
				ttstr(TJS_W("erasemacro")), (tjs_int)tag_erasemacro);
			special_tags_hash.Add(
				ttstr(TJS_W("jump")), (tjs_int)tag_jump);
			special_tags_hash.Add(
				ttstr(TJS_W("call")), (tjs_int)tag_call);
			special_tags_hash.Add(
				ttstr(TJS_W("return")), (tjs_int)tag_return);
		}


		tjs_int * tag = special_tags_hash.Find(tagname);
		tagkind = tag?(tSpecialTags)*tag:tag_other;

		if(tagkind == tag_macro) RecordingMacroName.Clear();


#define TVP_KAG_STEP_NEXT \
						if(ldelim == 0) \
						{ \
							CurLine++; \
							CurPos = 0; \
							LineBufferUsing = false; \
						} \
						else \
						{ \
							CurPos++; \
						}


		// tag attributes
		while(true)
		{
			while(TVPIsWS(CurLineStr[CurPos])) CurPos ++;

			if(CurLineStr[CurPos] == ldelim)
			{
				// tag ended

				bool ismacro = false;
				ttstr macrocontent;

				if(condition && ExcludeLevel == -1)
				{
					if(tagkind == tag_endmacro)
					{
						// macro recording ended
						// endmacro
						RecordingMacro = false;
						if(DebugLevel >= tkdlVerbose)
						{
							TVPAddLog(TJS_W("macro : ") + RecordingMacroName +
								TJS_W(" : ") + RecordingMacroStr);
						}

						RecordingMacroStr += TJS_W("[macropop]");
							// ensure macro arguments are to be popped

						// register macro
						tTJSVariant macrocontent(RecordingMacroStr);
						Macros->PropSet(TJS_MEMBERENSURE, RecordingMacroName.c_str(),
							NULL, &macrocontent, Macros);
					}

					// record macro
					if(RecordingMacro)
					{
						if(ldelim != 0)
						{
							// normal tag
							RecordingMacroStr += ttstr(CurLineStr + tagstart,
								CurPos - tagstart + 1);
						}
						else
						{
							// line command
							if(CurPos - tagstart >= 1)
							{
								RecordingMacroStr += TJS_W("[") +
									ttstr(CurLineStr + tagstart +1,
									CurPos - tagstart - 1) + TJS_W("]");
							}
						}

						TVP_KAG_STEP_NEXT;

						break; // break
					}


					// is macro ?
					tTJSVariant macroval;
					ismacro = TJS_SUCCEEDED(
						Macros->PropGet(0, tagname.c_str(), NULL, &macroval, Macros));
					if(ismacro) ismacro = macroval.Type() != tvtVoid;
					if(ismacro) macrocontent = ttstr(macroval);
				}

				// tag-specific processing
				if(tagkind == tag_other && !ismacro)
				{
					// not a control tag

					TVP_KAG_STEP_NEXT;

					if(condition && ExcludeLevel == -1)
					{
						DicObj->AddRef();
						return DicObj;
					}

					break;
				}

				// if/endif
				if(tagkind == tag_if || tagkind == tag_ignore)
				{
					tTJSVariant val;
					ttstr exp;
					DicObj->PropGet(0, __exp_name.c_str(), __exp_name.GetHint(), &val, DicObj);
					exp = val;
					if(exp == TJS_W(""))
						TVPThrowExceptionMessage(TVPKAGSyntaxError);
					TVPExecuteExpression(exp, &val);
					IfLevel ++;

					bool cond = val.operator bool();
					if(tagkind == tag_if) cond = !cond;

					if(cond)
					{
						if(ExcludeLevel == -1)
						{
							ExcludeLevel = IfLevel;
						}
					}
				}


				// endif/endignore
				if(tagkind == tag_endif || tagkind == tag_endignore)
				{
					// endif
					if(IfLevel == ExcludeLevel)
					{
						ExcludeLevel = -1;
					}

					IfLevel --;
					if(IfLevel < 0) IfLevel = 0;

					TVP_KAG_STEP_NEXT;

					break; // break
				}


				if(condition && ExcludeLevel == -1)
				{
					if(tagkind == tag_emb || (ismacro && tagkind==tag_other))
					{
						// embed string
						// insert string to current position
						if(ldelim != 0) CurPos++;

						if(!ismacro)
						{
							// execute expression
							tTJSVariant val;
							ttstr exp;
							DicObj->PropGet(0, __exp_name.c_str(), __exp_name.GetHint(), &val, DicObj);
							exp = val;
							if(exp == TJS_W(""))
								TVPThrowExceptionMessage(TVPKAGSyntaxError);
							TVPExecuteExpression(exp, &val);
							exp = val;

							// count '['
							const tjs_char *p = exp.c_str();
							tjs_int r_count = 0;
							while(*p)
							{
								if(*p == TJS_W('[')) r_count++;
								p++;
								r_count++;
							}

							tjs_int curposlen = TJS_strlen(CurLineStr + CurPos);
							tjs_int finallen = r_count + tagstartpos + curposlen;

							if(ldelim == 0 && !IgnoreCR) finallen++;

							ttstr newbuf;

							tjs_char *d = newbuf.AllocBuffer(finallen + 1);

							TJS_strncpy(d, CurLineStr, tagstartpos);
							d += tagstartpos;

							// escape '['
							p = exp.c_str();
							while(*p)
							{
								if(*p == TJS_W('['))
								{
									*d = TJS_W('['); d++;
									*d = TJS_W('['); d++;
								}
								else
								{
									*d = *p; d++;
								}
								p++;
							}

							TJS_strcpy(d, CurLineStr + CurPos);

							if(ldelim == 0 && !IgnoreCR)
							{
								d += curposlen;
								*d = TJS_W('\\'); d++;
								*d = 0;
							}

							newbuf.FixLen();
							LineBuffer = newbuf;
						}
						else
						{
							tjs_int maclen = macrocontent.GetLen();
							tjs_int curposlen = TJS_strlen(CurLineStr + CurPos);
							tjs_int finallen = tagstartpos + maclen + curposlen;

							if(ldelim == 0 && !IgnoreCR) finallen++;

							ttstr newbuf;
							tjs_char *d = newbuf.AllocBuffer(finallen + 1);

							TJS_strncpy(d, CurLineStr, tagstartpos);
							d += tagstartpos;
							TJS_strcpy(d, macrocontent.c_str());
							d += maclen;
							TJS_strcpy(d, CurLineStr + CurPos);

							if(ldelim == 0 && !IgnoreCR)
							{
								d += curposlen;
								*d = TJS_W('\\'); d++;
								*d = 0;
							}

							newbuf.FixLen();
							LineBuffer = newbuf;
						}

						CurLineStr = LineBuffer.c_str();
						CurPos = tagstartpos;

						LineBufferUsing = true;

						// push macro arguments
						if(ismacro) PushMacroArgs(DicObj);

						break;
					}
					else if(tagkind == tag_jump)
					{
						// jump tag
						ttstr attrib_storage;
						ttstr attrib_target;
						tTJSVariant val;
						DicObj->PropGet(0, __storage_name.c_str(), __storage_name.GetHint(), &val, DicObj);
						attrib_storage = val;
						DicObj->PropGet(0, __target_name.c_str(), __target_name.GetHint(), &val, DicObj);
						attrib_target = val;


						// fire onJump event
						bool process = true;
						if(Owner)
						{
							tTJSVariant param(DicObj, DicObj);
							tTJSVariant *pparam = &param;
							static ttstr event_name(TJSMapGlobalStringMap(TJS_W("onJump")));
							tTJSVariant res;
							tjs_error er = Owner->FuncCall(0, event_name.c_str(),
								event_name.GetHint(), &res, 1, &pparam, Owner);
							if(er == TJS_S_OK) process = res.operator bool();
						}


						if(process)
						{
							GoToStorageAndLabel(attrib_storage, attrib_target);
							goto parse_start; // re-start parsing
						}
					}
					else if(tagkind == tag_call)
					{
						// call tag
						ttstr attrib_storage;
						ttstr attrib_target;
						tTJSVariant val;
						DicObj->PropGet(0, __storage_name.c_str(), __storage_name.GetHint(), &val, DicObj);
						attrib_storage = val;
						DicObj->PropGet(0, __target_name.c_str(), __target_name.GetHint(), &val, DicObj);
						attrib_target = val;

						// fire onCall event
						bool process = true;
						if(Owner)
						{
							tTJSVariant param(DicObj, DicObj);
							tTJSVariant *pparam = &param;
							static ttstr event_name(TJSMapGlobalStringMap(TJS_W("onCall")));
							tTJSVariant res;
							tjs_error er = Owner->FuncCall(0, event_name.c_str(),
								event_name.GetHint(), &res, 1, &pparam, Owner);
							if(er == TJS_S_OK) process = res.operator bool();
						}


						if(process)
						{
							TVP_KAG_STEP_NEXT;

							PushCallStack();
							GoToStorageAndLabel(attrib_storage, attrib_target);
							goto parse_start;
						}
					}
					else if(tagkind == tag_return)
					{
						// return tag
						ttstr attrib_storage;
						ttstr attrib_target;
						tTJSVariant val;
						DicObj->PropGet(0, __storage_name.c_str(), __storage_name.GetHint(), &val, DicObj);
						attrib_storage = val;
						DicObj->PropGet(0, __target_name.c_str(), __target_name.GetHint(), &val, DicObj);
						attrib_target = val;

						// fire onReturn event
						bool process = true;
						if(Owner)
						{
							tTJSVariant param(DicObj, DicObj);
							tTJSVariant *pparam = &param;
							static ttstr event_name(TJSMapGlobalStringMap(TJS_W("onReturn")));
							tTJSVariant res;
							tjs_error er = Owner->FuncCall(0, event_name.c_str(),
								event_name.GetHint(), &res, 1, &pparam, Owner);
							if(er == TJS_S_OK) process = res.operator bool();
						}

						if(process)
						{
							PopCallStack(attrib_storage, attrib_target);
							goto parse_start;
						}
					}
					else
					{
						if(tagkind == tag_macro)
						{
							tTJSVariant val;
							DicObj->PropGet(0, TJS_W("name"), 0, &val, DicObj);
							RecordingMacroName = val;
							RecordingMacroName.ToLowerCase();
							if(RecordingMacroName == TJS_W(""))
								TVPThrowExceptionMessage(TVPKAGSyntaxError);
									// missing macro name
							RecordingMacro = true; // start recording macro
							RecordingMacroStr.Clear();
						}
						else if(tagkind == tag_macropop)
						{
							// pop macro arguments
							PopMacroArgs();
						}
						else if(tagkind == tag_erasemacro)
						{
							tTJSVariant val;
							DicObj->PropGet(0, TJS_W("name"), 0, &val, DicObj);
							ttstr macroname = val;
							if(TJS_FAILED(
								Macros->DeleteMember(0, macroname.c_str(), 0, Macros)))
								TVPThrowExceptionMessage(TVPUnknownMacroName, macroname);
						}
					}
				}

				TVP_KAG_STEP_NEXT;
				break;
			}

			if(CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);

			// attrib name
			if(CurLineStr[CurPos] == TJS_W('*'))
			{
				// macro entity all
				if(!RecordingMacro)
				{
					iTJSDispatch2 *dsp = GetMacroTopNoAddRef();
					if(dsp)
					{
						// assign macro arguments to current arguments
						tTJSVariant src(dsp, dsp);
						tTJSVariant *psrc = &src;
						DicAssign->FuncCall(0, NULL, NULL, NULL, 1, &psrc, DicObj);
					}
					tTJSVariant tag_val(tagname);
					DicObj->PropSetByVS(TJS_MEMBERENSURE,
						__tag_name.AsVariantStringNoAddRef(), &tag_val, DicObj);
							// reset tag_name
				}

				CurPos++;
				while(CurLineStr[CurPos] && TVPIsWS(CurLineStr[CurPos])) CurPos ++;
				continue;
			}

			const tjs_char *attribnamestart = CurLineStr + CurPos;
			while(CurLineStr[CurPos] && !TVPIsWS(CurLineStr[CurPos]) &&
				CurLineStr[CurPos] != TJS_W('='))
					CurPos ++;
			if(CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);

			const tjs_char *attribnameend = CurLineStr + CurPos;

			ttstr attribname(attribnamestart, attribnameend - attribnamestart);
			attribname.ToLowerCase();

			// =
			while(TVPIsWS(CurLineStr[CurPos])) CurPos ++;
			while(CurLineStr[CurPos] && CurLineStr[CurPos] != TJS_W('='))
					CurPos ++;
			if(CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);
			CurPos++;
			if(CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);
			while(CurLineStr[CurPos] && TVPIsWS(CurLineStr[CurPos])) CurPos ++;
			if(CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);

			// attrib value
			tjs_char vdelim = 0; // value delimiter
			bool entity = false;
			bool macroarg = false;

			if(CurLineStr[CurPos] == TJS_W('&'))
				entity = true, CurPos++;
			else if(CurLineStr[CurPos] == TJS_W('%'))
				macroarg = true, CurPos++;

			if(CurLineStr[CurPos] == TJS_W('\"') ||
				CurLineStr[CurPos] == TJS_W('\''))
			{
				vdelim = CurLineStr[CurPos];
				CurPos++;
			}

			const tjs_char *valuestart = CurLineStr + CurPos;

			while(CurLineStr[CurPos] &&
				(vdelim ? (CurLineStr[CurPos] != vdelim) :
					(CurLineStr[CurPos] != ldelim &&
						!TVPIsWS(CurLineStr[CurPos])) ) )
			{
				if(CurLineStr[CurPos] == TJS_W('`'))
				{
					// escaped with '`'
					CurPos++;
					if(CurLineStr[CurPos] == 0)
						TVPThrowExceptionMessage(TVPKAGSyntaxError);
				}
				CurPos++;
			}

			if(ldelim != 0 && CurLineStr[CurPos] == 0)
				TVPThrowExceptionMessage(TVPKAGSyntaxError);
			const tjs_char *valueend = CurLineStr + CurPos;

			if(vdelim) CurPos ++;

			// unescape ` character of value
			ttstr value(valuestart, valueend - valuestart);
			if(valueend != valuestart)
			{
				// value has at least one character
				tjs_char * vp = value.Independ();
				tjs_char * wvp = vp;

				if(!entity && *vp == TJS_W('&')) entity = true, vp++;
				if(!macroarg && *vp == TJS_W('%')) macroarg = true, vp++;

				while(*vp)
				{
					if(*vp == TJS_W('`'))
					{
						vp++;
						if(!*vp) break;
					}
					*wvp = *vp;
					vp++;
					wvp++;
				}
				*wvp = 0;
				value.FixLen();
			}


			// special attibute processing
			bool store = true;
			if(!RecordingMacro && ExcludeLevel == -1)
			{
				// process expression entity or macro argument
				if(entity)
				{
					TVPExecuteExpression(value, &ValueVariant);
					if(ValueVariant.Type() != tvtVoid) ValueVariant.ToString();
				}
				else if(macroarg)
				{
					iTJSDispatch2 *args = GetMacroTopNoAddRef();
					if(args)
					{
						tjs_char *vp = TJS_strchr(value.c_str(), TJS_W('|'));

						if(vp)
						{
							ttstr name(value.c_str(), vp - value.c_str());
							args->PropGet(0, name.c_str(), NULL, &ValueVariant, args);
							if(ValueVariant.Type() == tvtVoid)
									ValueVariant = ttstr(vp + 1);
						}
						else
						{
							args->PropGet(0, value.c_str(), NULL, &ValueVariant, args);
						}

					}
					else
					{
						ValueVariant = value;
					}
				}
				else
				{
					ValueVariant = value;
				}

				if(attribname == TJS_W("cond"))
				{
					// condition

					tTJSVariant val;
					TVPExecuteExpression(ttstr(ValueVariant), &val);
					condition = val.operator bool();
					store = false;
				}
			}

			// store value into the dictionary object
			if(store)
				DicObj->PropSetByVS(TJS_MEMBERENSURE,
					attribname.AsVariantStringNoAddRef(), &ValueVariant, DicObj);
		}
	}

	return NULL;
}
//---------------------------------------------------------------------------
iTJSDispatch2 * tTJSNI_KAGParser::GetNextTag()
{
	return _GetNextTag();
}
//---------------------------------------------------------------------------
iTJSDispatch2 *tTJSNI_KAGParser::GetMacroTopNoAddRef() const
{
	if(MacroArgStackDepth == 0) return NULL;

	return MacroArgs[MacroArgStackDepth - 1];
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTJSNC_KAGParser : KAGParser TJS native class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_KAGParser::ClassID = (tjs_uint32)-1;
tTJSNC_KAGParser::tTJSNC_KAGParser() :
	tTJSNativeClass(TJS_W("KAGParser"))
{
	// register native methods/properties

	TJS_BEGIN_NATIVE_MEMBERS(KAGParser)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
// constructor/methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_KAGParser,
	/*TJS class name*/KAGParser)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/KAGParser)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/loadScenario)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	_this->LoadScenario(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/loadScenario)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/goToLabel)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	_this->GoToLabel(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/goToLabel)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/callLabel)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	_this->CallLabel(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/callLabel)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getNextTag)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
//	if(numparams < 0) return TJS_E_BADPARAMCOUNT;
	iTJSDispatch2 *dsp = _this->GetNextTag();
	if(dsp == NULL)
	{
		if(result) result->Clear(); // return void ( not null )
	}
	else
	{
		if(result) *result = tTJSVariant(dsp, dsp);
		dsp->Release();
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/getNextTag)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/assign)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	tTJSNI_KAGParser *src;
	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.Object)
	{
		if(TJS_FAILED(clo.Object->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
			tTJSNC_KAGParser::ClassID, (iTJSNativeInstance**)&src)))
			TVPThrowExceptionMessage(TVPKAGSpecifyKAGParser);
	}
	else
	{
		TVPThrowExceptionMessage(TVPKAGSpecifyKAGParser);
	}

	_this->Assign(*src);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/assign)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/clear)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	_this->Clear();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/clear)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/store)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	iTJSDispatch2 * dsp = _this->Store();
	if(result) *result = tTJSVariant(dsp, dsp);
	dsp->Release();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/store)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/restore)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	iTJSDispatch2 * dsp = param[0]->AsObjectNoAddRef();

	_this->Restore(dsp);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/restore)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/clearCallStack)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	_this->ClearCallStack();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/clearCallStack)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/interrupt)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	_this->Interrupt();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/interrupt)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/resetInterrupt)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);

	_this->ResetInterrupt();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/resetInterrupt)
//----------------------------------------------------------------------

//---------------------------------------------------------------------------




//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(curLine)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = _this->GetCurLine();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(curLine)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(curPos)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = _this->GetCurPos();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(curPos)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(curLineStr)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = ttstr(_this->GetCurLineStr());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(curLineStr)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(ignoreCR)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = (tjs_int)_this->GetIgnoreCR();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		_this->SetIgnoreCR(param->operator bool());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(ignoreCR)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(debugLevel)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = (tjs_int)_this->GetDebugLevel();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		_this->SetDebugLevel((tTVPKAGDebugLevel)(tjs_int)(*param));
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(debugLevel)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(macros)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		iTJSDispatch2 *macros = _this->GetMacrosNoAddRef();
		*result = tTJSVariant(macros, macros);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(macros)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(macroParams)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		iTJSDispatch2 *params = _this->GetMacroTopNoAddRef();
		*result = tTJSVariant(params, params);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(macroParams)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(callStackDepth)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = _this->GetCallStackDepth();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(callStackDepth)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(curStorage)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = _this->GetStorageName();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		_this->LoadScenario(*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(curStorage)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(curLabel)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_KAGParser);
		*result = _this->GetCurLabel();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(curLabel)
//---------------------------------------------------------------------------

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
iTJSNativeInstance *tTJSNC_KAGParser::CreateNativeInstance()
{
	return new tTJSNI_KAGParser();
}
//---------------------------------------------------------------------------



