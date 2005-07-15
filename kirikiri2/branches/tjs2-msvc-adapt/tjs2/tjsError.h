//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS2's C++ exception class and exception message
//---------------------------------------------------------------------------


#ifndef tjsErrorH
#define tjsErrorH

#ifndef TJS_DECL_MESSAGE_BODY

#include <stdexcept>
#include <string>
#include "tjsVariant.h"
#include "tjsString.h"
#include "tjsMessage.h"

namespace TJS
{
//---------------------------------------------------------------------------
extern ttstr TJSNonamedException;

//---------------------------------------------------------------------------
// macro
//---------------------------------------------------------------------------
#ifdef TJS_SUPPORT_VCL
	#define TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL \
		catch(const Exception &e) \
		{ \
			TJS::TJS_eTJSError(e.Message.c_str()); \
		}
#else
	#define TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL
#endif


#define TJS_CONVERT_TO_TJS_EXCEPTION \
	catch(const eTJSSilent &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSScriptException &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSScriptError &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSError &e) \
	{ \
		throw e; \
	} \
	catch(const eTJS &e) \
	{ \
		TJS::TJS_eTJSError(e.GetEMessage()); \
	} \
	catch(const std::exception &e) \
	{ \
		TJS::TJS_eTJSError(e.what()); \
	} \
	catch(const wchar_t *text) \
	{ \
		TJS::TJS_eTJSError(text); \
	} \
	catch(const char *text) \
	{ \
		TJS::TJS_eTJSError(text); \
	} \
	TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSGetExceptionObject : retrieves TJS 'Exception' object
//---------------------------------------------------------------------------
extern void TJSGetExceptionObject(tTJS *tjs, tTJSVariant *res, tTJSVariant &msg,
	tTJSVariant *trace/* trace is optional */ = NULL);
//---------------------------------------------------------------------------

#ifdef TJS_SUPPORT_VCL
	#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(_tjs, _result_condition, _result_addr, _before_catched, _when_catched) \
	catch(EAccessViolation &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tTJSVariant msg(e.Message.c_str()); \
			TJSGetExceptionObject((_tjs), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	} \
	catch(Exception &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tTJSVariant msg(e.Message.c_str()); \
			TJSGetExceptionObject((_tjs), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	}
#else
	#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(_tjs, _result_condition, _result_addr, _before_catched, _when_catched)
#endif


#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT(tjs, result_condition, result_addr, before_catched, when_catched) \
	catch(eTJSSilent &e) \
	{ \
		throw e; \
	} \
	catch(eTJSScriptException &e) \
	{ \
		before_catched \
		if(result_condition) *(result_addr) = e.GetValue(); \
		when_catched; \
	} \
	catch(eTJSScriptError &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.GetEMessage()); \
			tTJSVariant trace(e.GetTrace()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, &trace); \
		} \
		when_catched; \
	} \
	catch(eTJS &e)  \
	{  \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.GetEMessage()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	catch(exception &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.what()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(tjs, result_condition, result_addr, before_catched, when_catched) \
	catch(...) \
	{ \
		before_catched \
		if(result_condition) (result_addr)->Clear(); \
		when_catched; \
	}




//---------------------------------------------------------------------------
// eTJSxxxxx
//---------------------------------------------------------------------------
class eTJSSilent
{
	// silent exception
};
//---------------------------------------------------------------------------
class eTJS
{
public:
	eTJS() {;}
	eTJS(const eTJS&) {;}
	eTJS& operator= (const eTJS& e) { return *this; }
	virtual ~eTJS() {;}
	virtual const ttstr & GetEMessage() const 
	{ return TJSNonamedException; }
};
//---------------------------------------------------------------------------
void TJS_eTJS();
//---------------------------------------------------------------------------
class eTJSError : public eTJS
{
public:
	eTJSError(const ttstr & Msg) :
		Message(Msg) {;}
	const ttstr & GetEMessage() const { return Message; }

	void AppendMessage(const ttstr & msg) { Message += msg; }

private:
	ttstr Message;
};
//---------------------------------------------------------------------------
void TJS_eTJSError(const ttstr & msg);
void TJS_eTJSError(const tjs_char* msg);
//---------------------------------------------------------------------------
class eTJSVariantError : public eTJSError
{
public:
	eTJSVariantError(const ttstr & Msg) :
		eTJSError(Msg) {;}

	eTJSVariantError(const eTJSVariantError &ref) :
		eTJSError(ref) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSVariantError(const ttstr & msg);
void TJS_eTJSVariantError(const tjs_char * msg);
//---------------------------------------------------------------------------
class tTJSScriptBlock;
class tTJSInterCodeContext;
class eTJSScriptError : public eTJSError
{
	class tScriptBlockHolder
	{
	public:
		tScriptBlockHolder(tTJSScriptBlock *block);
		~tScriptBlockHolder();
		tScriptBlockHolder(const tScriptBlockHolder &holder);
		tTJSScriptBlock *Block;
	} Block;

	tjs_int Position;

	ttstr Trace;

public:
	tTJSScriptBlock * GetBlockNoAddRef() { return Block.Block; }

	tjs_int GetPosition() const { return Position; }

	tjs_int GetSourceLine() const;

	const tjs_char * GetBlockName() const;

	const ttstr & GetTrace() const { return Trace; }

	bool AddTrace(tTJSScriptBlock *block, tjs_int srcpos);
	bool AddTrace(tTJSInterCodeContext *context, tjs_int codepos);
	bool AddTrace(const ttstr & data);

	eTJSScriptError(const ttstr &  Msg,
		tTJSScriptBlock *block, tjs_int pos);

	eTJSScriptError(const eTJSScriptError &ref) :
		eTJSError(ref), Block(ref.Block), Position(ref.Position), Trace(ref.Trace) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const ttstr &msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSScriptError(const tjs_char *msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSScriptError(const ttstr &msg, tTJSInterCodeContext *context, tjs_int codepos);
void TJS_eTJSScriptError(const tjs_char *msg, tTJSInterCodeContext *context, tjs_int codepos);
//---------------------------------------------------------------------------
class eTJSScriptException : public eTJSScriptError
{
	tTJSVariant Value;
public:
	tTJSVariant & GetValue() { return Value; }

	eTJSScriptException(const ttstr & Msg,
		tTJSScriptBlock *block, tjs_int pos, tTJSVariant &val)
			: eTJSScriptError(Msg, block, pos), Value(val) {}

	eTJSScriptException(const eTJSScriptException &ref) :
		eTJSScriptError(ref), Value(ref.Value) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const ttstr &msg, tTJSScriptBlock *block,
	tjs_int srcpos, tTJSVariant &val);
void TJS_eTJSScriptException(const tjs_char *msg, tTJSScriptBlock *block,
	tjs_int srcpos, tTJSVariant &val);
void TJS_eTJSScriptException(const ttstr &msg, tTJSInterCodeContext *context,
	tjs_int codepos, tTJSVariant &val);
void TJS_eTJSScriptException(const tjs_char *msg, tTJSInterCodeContext *context,
	tjs_int codepos, tTJSVariant &val);
//---------------------------------------------------------------------------
class eTJSCompileError : public eTJSScriptError
{
public:
	eTJSCompileError(const ttstr &  Msg, tTJSScriptBlock *block, tjs_int pos) :
		eTJSScriptError(Msg, block, pos) {;}

	eTJSCompileError(const eTJSCompileError &ref) : eTJSScriptError(ref) {;}

};
//---------------------------------------------------------------------------
void TJS_eTJSCompileError(const ttstr & msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSCompileError(const tjs_char * msg, tTJSScriptBlock *block, tjs_int srcpos);
//---------------------------------------------------------------------------
void TJSThrowFrom_tjs_error(tjs_error hr, const tjs_char *name = NULL);
#define TJS_THROW_IF_ERROR(x) { \
	tjs_error ____er; ____er = (x); if(TJS_FAILED(____er)) TJSThrowFrom_tjs_error(____er); }
//---------------------------------------------------------------------------
} // namespace TJS
//---------------------------------------------------------------------------
#endif // #ifndef TJS_DECL_MESSAGE_BODY



//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// messages
//---------------------------------------------------------------------------
namespace TJS
{
#ifdef TJS_DECL_MESSAGE_BODY
	#define TJS_MSG_DECL(name, msg) tTJSMessageHolder name(TJS_W(#name), TJS_W(msg));
#else
	#define TJS_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif
//---------------------------------------------------------------------------
#ifdef TJS_JP_LOCALIZED
	#include "tjsError_jp.h"
#else
TJS_MSG_DECL(TJSInternalError, "Internal error")
TJS_MSG_DECL(TJSWarning, "Warning: ")
TJS_MSG_DECL(TJSWarnEvalOperator, "Non-global post-! operator is used (note that the post-! operator behavior is changed on TJS2 version 2.4.1)")
TJS_MSG_DECL(TJSNarrowToWideConversionError, "Cannot convert given narrow string to wide string")
TJS_MSG_DECL(TJSVariantConvertError, "Cannot convert the variable type (%1 to %2)")
TJS_MSG_DECL(TJSVariantConvertErrorToObject, "Cannot convert the variable type (%1 to Object)")
TJS_MSG_DECL(TJSIDExpected, "Specify an ID")
TJS_MSG_DECL(TJSCannotModifyLHS, "This expression cannot be used as a lvalue")
TJS_MSG_DECL(TJSInsufficientMem, "Insufficient memory")
TJS_MSG_DECL(TJSCannotGetResult, "Cannot get the value of this expression")
TJS_MSG_DECL(TJSNullAccess, "Accessing to null object")
TJS_MSG_DECL(TJSMemberNotFound, "Member \"%1\" does not exist")
TJS_MSG_DECL(TJSMemberNotFoundNoNameGiven, "Member does not exist")
TJS_MSG_DECL(TJSNotImplemented, "Called method is not implemented")
TJS_MSG_DECL(TJSInvalidParam, "Invalid argument")
TJS_MSG_DECL(TJSBadParamCount, "Invalid argument count")
TJS_MSG_DECL(TJSInvalidType, "Not a function or invalid method/property type")
TJS_MSG_DECL(TJSSpecifyDicOrArray, "Specify a Dictionary object or an Array object");
TJS_MSG_DECL(TJSSpecifyArray, "Specify an Array object");
TJS_MSG_DECL(TJSStringDeallocError, "Cannot free the string memory block")
TJS_MSG_DECL(TJSStringAllocError, "Cannot allocate the string memory block")
TJS_MSG_DECL(TJSMisplacedBreakContinue, "Cannot place \"break\" or \"continue\" here")
TJS_MSG_DECL(TJSMisplacedCase, "Cannot place \"case\" here")
TJS_MSG_DECL(TJSMisplacedReturn, "Cannot place \"return\" here")
TJS_MSG_DECL(TJSStringParseError, "Un-terminated string/regexp/octet literal")
TJS_MSG_DECL(TJSNumberError, "Cannot be parsed as a number")
TJS_MSG_DECL(TJSUnclosedComment, "Un-terminated comment")
TJS_MSG_DECL(TJSInvalidChar, "Invalid character \'%1\'")
TJS_MSG_DECL(TJSExpected, "Expected %1")
TJS_MSG_DECL(TJSSyntaxError, "Syntax error (%1)")
TJS_MSG_DECL(TJSPPError, "Error in conditional compiling expression")
TJS_MSG_DECL(TJSCannotGetSuper, "Super class does not exist or cannot specify the super class")
TJS_MSG_DECL(TJSInvalidOpecode, "Invalid VM code")
TJS_MSG_DECL(TJSRangeError, "The value is out of the range")
TJS_MSG_DECL(TJSAccessDenyed, "Invalid operation for Read-only or Write-only property")
TJS_MSG_DECL(TJSNativeClassCrash, "Invalid object context")
TJS_MSG_DECL(TJSInvalidObject, "The object is already invalidated")
TJS_MSG_DECL(TJSDuplicatedPropHandler, "Duplicated \"setter\" or \"getter\"")
TJS_MSG_DECL(TJSCannotOmit, "\"...\" is used out of functions")
TJS_MSG_DECL(TJSCannotParseDate, "Invalid date format")
TJS_MSG_DECL(TJSInvalidValueForTimestamp, "Invalid value for date/time")
TJS_MSG_DECL(TJSExceptionNotFound, "Cannot convert exception because \"Exception\" does not exist")
TJS_MSG_DECL(TJSInvalidFormatString, "Invalid format string")
TJS_MSG_DECL(TJSDivideByZero, "Division by zero")
TJS_MSG_DECL(TJSNotReconstructiveRandomizeData, "Cannot reconstruct random seeds")
TJS_MSG_DECL(TJSSymbol, "ID")
TJS_MSG_DECL(TJSCallHistoryIsFromOutOfTJS2Script, "[out of TJS2 script]")
TJS_MSG_DECL(TJSNObjectsWasNotFreed, "Total %1 Object(s) was not freed")
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, " <-- ");
TJS_MSG_DECL(TJSObjectWasNotFreed, "Object %1 [%2] wes not freed / The object was created at : %2")
TJS_MSG_DECL(TJSGroupByObjectTypeAndHistory, "Group by object type and location where the object was created")
TJS_MSG_DECL(TJSGroupByObjectType, "Group by object type")
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory, "%1 time(s) : [%2] %3")
TJS_MSG_DECL(TJSObjectCountingMessageTJSGroupByObjectType, "%1 time(s) : [%2]")
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, "%4: Running code on deleting-in-progress object %1[%2] / The object was created at : %3")
TJS_MSG_DECL(TJSWriteError, "Write error")
TJS_MSG_DECL(TJSReadError, "Read error")
TJS_MSG_DECL(TJSSeekError, "Seek error")
#endif

#undef TJS_MSG_DECL
//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------


#endif // #ifndef tjsErrorH




