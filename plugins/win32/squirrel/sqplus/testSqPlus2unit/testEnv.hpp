#ifndef TESTENV_HPP
#define TESTENV_HPP

#ifdef WIN32
# pragma warning(disable : 4996)
#endif

#include <sqplus.h>
#include <stdio.h>
#include <UnitTest++.h>

struct TestEnv {
    struct debug_info_t;
    static void report_exception(void);
    static struct debug_info_t &debug(void);
    TestEnv(void);
    ~TestEnv();
};

struct TestEnv::debug_info_t {
    SQUserPointer up;
    int event_type, line;
    const SQChar *src, *func;
    const SQChar *script;
    debug_info_t(void) {reset();}
    void reset(void) {script = NULL;}
};

const char *toMultibyte(const wchar_t *orig);

namespace UnitTest {
template<> inline void
CheckEqual<const wchar_t *, const wchar_t *>(
    TestResults& results,
    const wchar_t *expected,
    const wchar_t *actual,
    TestDetails const& details
    )
{
    if (!wcsstr(expected, actual)) {
        UnitTest::MemoryOutStream stream;
        stream << "Expected " << toMultibyte(expected)
               << " but was " << toMultibyte(actual);
        results.OnTestFailure(details, stream.GetText());
    }
}
}

#define RUN_SCRIPT(s) \
try {\
    TestEnv::debug().script = s;\
    SquirrelVM::RunScript(SquirrelVM::CompileBuffer(TestEnv::debug().script));\
}\
catch (...) {\
    TestEnv::report_exception();\
    CHECK(false);\
}

#define RUN_SCRIPT_THROW(s, d) \
try {\
    TestEnv::debug().script = s;\
    SquirrelVM::RunScript(SquirrelVM::CompileBuffer(TestEnv::debug().script));\
}\
catch (const SquirrelError &e) {\
    CHECK_EQUAL(e.desc, d);\
}\
catch(...) {\
    CHECK(false);\
}

#define SQPLUS_TEST(s) TEST_FIXTURE(TestEnv, s)
#define SQPLUS_TEST_TRACE() printf("TESTING: %s\n", m_details.testName)
#define SQPLUS_TEST_TRACE_SUB(s) printf("\nTESTING: %s (in %s)\n", #s, m_details.testName)



#endif // TESTENV_HPP
