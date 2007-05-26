#include "testEnv.hpp"

#include <iostream>

#include <sqplus.h>
#include <sqstdaux.h>

#define SQDBG_DEBUG_HOOK _T("_sqdebughook_")
#define SQDBG_ERROR_HANDLER _T("_sqerrorhandler_")

#define PRINTBUFSIZE 4096

int
my_error_handler(HSQUIRRELVM v)
{
	const SQChar *sErr = NULL;
    
    if(sq_gettop(v) >= 1) {
        if(SQ_SUCCEEDED(sq_getstring(v, 2, &sErr)))	{
            scprintf(_T("Error detected: [%s]\n"), sErr);
        } else{
            scprintf(_T("Error detected: [unknown]\n"));
        }
        /* no callstack printing */
    }
	return 0;
}

int
debug_hook(HSQUIRRELVM v)
{
    struct TestEnv::debug_info_t &d = TestEnv::debug();
    sq_getinteger(v, 2, &d.event_type);
    sq_getstring(v, 3, &d.src);
    sq_getinteger(v, 4, &d.line);
    sq_getstring(v, 5, &d.func);
    sq_getuserpointer(v, -1, &d.up);
    return 0;
}




TestEnv::TestEnv(void)
{
    scprintf(_T("TEST BEGINS -------------------------------------->\n"));
    
    SquirrelVM::Init();
    HSQUIRRELVM _v = SquirrelVM::GetVMPtr();

    sq_pushregistrytable(_v);
    sq_pushstring(_v, SQDBG_DEBUG_HOOK, -1);
    sq_pushuserpointer(_v, this);
    sq_newclosure(_v, debug_hook, 1);
    sq_createslot(_v, -3);
    sq_pushstring(_v, SQDBG_DEBUG_HOOK, -1);
    sq_rawget(_v, -2);
    sq_setdebughook(_v);
    sq_pop(_v, 1);

    sq_pushregistrytable(_v);
    sq_pushstring(_v, SQDBG_ERROR_HANDLER, -1);
    sq_pushuserpointer(_v, this);
    sq_newclosure(_v, my_error_handler, 1);
    sq_createslot(_v, -3);
    sq_pushstring(_v, SQDBG_ERROR_HANDLER, -1);
    sq_rawget(_v, -2);
    sq_seterrorhandler(_v);
    sq_pop(_v, 1);

    sq_enabledebuginfo(_v, SQTrue);
}

TestEnv::~TestEnv()
{
    SquirrelVM::Shutdown();
    debug().reset();
    scprintf(_T("------------------------------------> TEST FINISHES\n\n"));
}

struct TestEnv::debug_info_t &
TestEnv::debug(void)
{
    static struct debug_info_t d;
    return d;
}

void
TestEnv::report_exception(void)
{
    try {
        throw;
    }
    catch (const SquirrelError &e) {
        scprintf(
            _T("\nSQUIRREL ERROR!\n[%s]\n"
                ) _T("At line (%d) in script buffer:\n>>>>>>>>>>>>>>\n"),
            e.desc,
            debug().line
            );

        const SQChar *c = debug().script;
        for (int i = 1; i < debug().line; ++i) {
            c = scstrstr(c, _T("\n"));
            if (!c) goto FINISH_OUTPUT_LINE;
            ++c;
        }
        for (; *c != _T('\0') && *c != _T('\n'); ++c) {
            putchar(*c);
        }
      FINISH_OUTPUT_LINE:
        scprintf(_T("\n<<<<<<<<<<<<<<\n"));
    }
}


const char *
toMultibyte(const wchar_t *orig)
{
    using namespace std;
    static char dst[BUFSIZ];
    const size_t length = min(wcslen(orig) + 1, size_t(BUFSIZ));
    size_t convertedChars = 0;
    wcstombs(dst, orig, length);
    return dst;
}
