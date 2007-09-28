#include "testEnv.hpp"

/*
  http://squirrel-lang.org/forums/thread/1875.aspx
  http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SqPlusNativeCreatedInstancesWithCorrectAncestry.html
*/

namespace {

class cBase {
public:
    cBase(void) : myId(id++) {}
    virtual ~cBase() {}

    void echo(void) {printf("cBase(id = %d)\n", myId);}
    const SQChar *Name(void) {return _T("cBase");}
    const SQChar *BaseSpecific(void) {return _T("BaseSpecific(void)");}

    int myId;
    static int id;
};

class cDerived : public cBase {
public:
    cDerived(void) : cBase(), myId(id++) {}
    virtual ~cDerived() {}

    void echo(void) {
        printf("cDerived(id = %d), cBase(id = %d)\n", myId, cBase::myId);
    }
    const SQChar *Name(void) {return _T("cDerived");}
    const SQChar *DerivedSpecific(void) {return _T("DerivedSpecific(void)");}

    int myId;
    static int id;
};

int cBase::id = 0;
int cDerived::id = 1000;

}

DECLARE_INSTANCE_TYPE(cBase);
DECLARE_INSTANCE_TYPE(cDerived);


cBase b;
cDerived d;

cBase &GetBase() {return b;}
cDerived &GetDerived() {return d;}
cBase *GetBasePtr() {return &b;}
cDerived *GetDerivedPtr() {return &d;}


SQPLUS_TEST(Test_PointerToDerived)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    
#ifdef SQ_USE_CLASS_INHERITANCE

    SQClassDef<cBase>(_T("cBase"))
        .func(&cBase::echo, _T("echo"))
        .func(&cBase::Name, _T("Name"))
        .func(&cBase::BaseSpecific, _T("BaseSpecific"));

    SQClassDef<cDerived>(_T("cDerived"), _T("cBase"))
        .func(&cDerived::echo, _T("echo"))
        .func(&cDerived::Name, _T("Name"))
        .func(&cDerived::DerivedSpecific, _T("DerivedSpecific"));

    RegisterGlobal(&GetBase, _T("GetBase"));
    RegisterGlobal(&GetDerived, _T("GetDerived"));
    RegisterGlobal(&GetBasePtr, _T("GetBasePtr"));
    RegisterGlobal(&GetDerivedPtr, _T("GetDerivedPtr"));


    SQPLUS_TEST_TRACE_SUB(b <- cBase());
    RUN_SCRIPT(_T("\n\
b <- cBase(); \n\
b.echo() \n\
assert(b.Name() == \"cBase\"); \n\
assert(b.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(b <- GetBase());
    RUN_SCRIPT(_T("\n\
b <- GetBase(); \n\
b.echo() \n\
assert(b.Name() == \"cBase\"); \n\
assert(b.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(b <- GetBasePtr());
    RUN_SCRIPT(_T("\n\
b <- GetBasePtr(); \n\
b.echo() \n\
assert(b.Name() == \"cBase\"); \n\
assert(b.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(d <- cDerived());
    RUN_SCRIPT(_T("\n\
d <- cDerived(); \n\
d.echo() \n\
assert(d.Name() == \"cDerived\"); \n\
assert(d.DerivedSpecific() == \"DerivedSpecific(void)\"); \n\
assert(d.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(d <- GetDerived());
    RUN_SCRIPT(_T("\n\
d <- GetDerived(); \n\
d.echo() \n\
assert(d.Name() == \"cDerived\"); \n\
assert(d.DerivedSpecific() == \"DerivedSpecific(void)\"); \n\
assert(d.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(d <- GetDerivedPtr());
    RUN_SCRIPT(_T("\n\
d <- GetDerivedPtr(); \n\
d.echo() \n\
assert(d.Name() == \"cDerived\"); \n\
assert(d.DerivedSpecific() == \"DerivedSpecific(void)\"); \n\
assert(d.BaseSpecific() == \"BaseSpecific(void)\"); \n\
"));
#endif
}
