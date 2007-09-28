#include "testEnv.hpp"

/*
  Embedding a global function as an 'adaptor' to a member function (by ATS)
  http://squirrel-lang.org/forums/thread/2147.aspx
  
  In essence, it's a global function, but SqPlus injects 'this' as a
  first argument to the function.
 */
namespace {

class TestClass {
public:
    TestClass( ) : m_i(0) {}
    
    int AddOrSub( int a1, bool do_add ){
        return do_add ? a1+m_i : a1-m_i;
    }

    int    m_i; 
};

int TestClass_Add( TestClass *ptc, int a1 ){
    return ptc->AddOrSub( a1, true );
}

}

DECLARE_INSTANCE_TYPE(TestClass);


SQPLUS_TEST(Test_GlobalAsMember)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;

    SQClassDef<TestClass>(_T("TestClass"))
        .globMembFunc(&TestClass_Add, _T("Add"))
        .var(&TestClass::m_i, _T("m_i"));

    // Test that it works 
    // SqPlus injects an additional 'this' argument to global func
    RUN_SCRIPT(_T("\n\
tc <- TestClass(); \n\
tc.m_i = 200; \n\
print(\"GlobalAsMember: \"); \n\
print(tc.m_i); \n\
print(tc.Add(114)); \n\
assert(tc.Add(114)==314); \n\
"));
}
