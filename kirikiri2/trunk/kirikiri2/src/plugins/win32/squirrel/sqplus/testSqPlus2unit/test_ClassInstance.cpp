#include "testEnv.hpp"

#include "globals.hpp"
#include "newTestObj.hpp"
#include "constants.hpp"


// Using global functions to construct and release classes.
int
releaseNewTestObj(SQUserPointer up, SQInteger size)
{
    SQ_DELETE_CLASS(NewTestObj);
} // releaseNewTestObj

int
constructNewTestObj(HSQUIRRELVM v)
{
    using namespace SqPlus;
    StackHandler sa(v);
    int paramCount = sa.GetParamCount();
    if (paramCount == 1) {
        return PostConstruct<NewTestObj>(v,
                                         new NewTestObj(),
                                         releaseNewTestObj);
    } else if (paramCount == 4) {
        return PostConstruct<NewTestObj>(v,
                                         new NewTestObj(
                                             sa.GetString(2),
                                             sa.GetInt(3),
                                             sa.GetBool(4)? true: false
                                             ),
                                         releaseNewTestObj);
    } // if
    return sq_throwerror(v, _T("Invalid Constructor arguments"));
} // constructNewTestObj


// Using fixed args with auto-marshaling. Note that the HSQUIRRELVM
// must be last in the argument list (and must be present to send to
// PostConstruct).
//
// SquirrelVM::GetVMPtr() could also be used with PostConstruct(): no
// HSQUIRRELVM argument would be required.
int
constructNewTestObjFixedArgs(const SQChar *s, int val, bool b, HSQUIRRELVM v)
{
    using namespace SqPlus;
    StackHandler sa(v);
    int paramCount = sa.GetParamCount();
    return PostConstruct<NewTestObj>(v,
                                     new NewTestObj(s,val,b),
                                     releaseNewTestObj);
} // constructNewTestObj



struct CustomTestObj {
    SqPlus::ScriptStringVar128 name;
    int val;
    bool state;
    CustomTestObj() : val(0), state(false) {name = _T("empty");}
    CustomTestObj(const SQChar *_name, int _val, bool _state) : val(_val), state(_state) {
        name = _name;
    }

    // Custom variable argument constructor
    static int construct(HSQUIRRELVM v) {
        using namespace SqPlus;
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (paramCount == 1) {
            return PostConstruct<CustomTestObj>(v,
                                                new CustomTestObj(),
                                                release);
        } if (paramCount == 4) {
            return PostConstruct<CustomTestObj>(
                v,
                new CustomTestObj(sa.GetString(2),
                                  sa.GetInt(3),
                                  sa.GetBool(4) ? true : false),
                release
                );
        } // if
        return sq_throwerror(v,_T("Invalid Constructor arguments"));
    } // construct

    SQ_DECLARE_RELEASE(CustomTestObj); // Required when using a custom
                                       // constructor.
    
    // Member function that handles variable types.
    int varArgTypes(HSQUIRRELVM v) {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        if (sa.GetType(2) == OT_INTEGER) {
            val = sa.GetInt(2);
        } // if
        if (sa.GetType(2) == OT_STRING) {
            name = sa.GetString(2);
        } // if
        if (sa.GetType(3) == OT_INTEGER) {
            val = sa.GetInt(3);
        } // if
        if (sa.GetType(3) == OT_STRING) {
            name = sa.GetString(3);
        } // if
        //return sa.ThrowError(_T("varArgTypes() error"));
        return 0;
    } // varArgTypes

    // Member function that handles variable types and has variable
    // return types+count.
    int varArgTypesAndCount(HSQUIRRELVM v) {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        SQObjectType type1 = (SQObjectType)sa.GetType(1); // Always OT_INSTANCE
        SQObjectType type2 = (SQObjectType)sa.GetType(2);
        SQObjectType type3 = (SQObjectType)sa.GetType(3);
        SQObjectType type4 = (SQObjectType)sa.GetType(4);
        int returnCount = 0;
        if (paramCount == 3) {
            sq_pushinteger(v, val);
            returnCount = 1;
        } else if (paramCount == 4) {
            sq_pushinteger(v, val);
            sq_pushstring(v, name, -1);
            returnCount = 2;
        } // if
        return returnCount;
    } // varArgTypesAndCount

    int noArgsVariableReturn(HSQUIRRELVM v) {
        if (val == 123) {
            val++;
            return 0; // This will print (null).
        } else if (val == 124) {
            sq_pushinteger(v, val); // Will return int:124.
            val++;
            return 1;
        } else if (val == 125) {
            sq_pushinteger(v, val);
            name = _T("Case 125");
            sq_pushstring(v, name, -1);
            val = 123; // reset
            return 2;
        } // if
        return 0;
    } // noArgsVariableReturn

    // Registered with func() instead of funcVarArgs(): fixed (single)
    // return type.
    const SQChar *variableArgsFixedReturnType(HSQUIRRELVM v) {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        SQObjectType type1 = (SQObjectType)sa.GetType(1); // Always OT_INSTANCE
        SQObjectType type2 = (SQObjectType)sa.GetType(2);
        SQObjectType type3 = (SQObjectType)sa.GetType(3);
        if (paramCount == 1) {
            return _T("No Args");
        } else if (paramCount == 2) {
            return _T("One Arg");
        } else if (paramCount == 3) {
            return _T("Two Args");
        } // if
        return _T("More than two args");
    } // variableArgsFixedReturnType

    void manyArgs(int i, float f, bool b, const SQChar *s) {
        scprintf(_T("CustomTestObj::manyArgs() i: %d, f: %f, b: %s, s: %s\n"),
                 i, f, b ?_T("true") : _T("false"), s);
    } // manyArgs

    float manyArgsR1(int i, float f, bool b, const SQChar *s)  {
        manyArgs(i, f, b, s);
        return i + f;
    } // manyArgsR1
}; // struct CustomTestObj


class TestBase {
public:
    int x;
    TestBase() : x(0) {
        printf("Constructing TestBase[0x%x]\n", (size_t)this);
    }
    void print(void) {
        printf("TestBase[0x%x], x[%d]\n", (size_t)this, x);
    }
}; // class TestBase
DECLARE_INSTANCE_TYPE(TestBase);

class TestDerivedCPP : public TestBase {
public:
    int y;
    TestDerivedCPP() {
        x = 121;
    }
}; // class TestDerivedCPP

typedef void (TestDerivedCPP::*TestDerivedCPP_print)(void);




SQPLUS_TEST(Test_ClassInstance)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    SquirrelObject root = SquirrelVM::GetRootTable();


    // === BEGIN Class Instance tests ===

    // Example showing two methods for registration.
    SQClassDef<NewTestObj>(_T("NewTestObj"))
        // If a special constructor+destructor are not needed, the
        // auto-generated versions can be used.
        
        // Example methods for custom constructors:
        
        /* Using a global constructor: useful in cases where a custom
           constructor/destructor are required and the original class
           is not to be modified. */
        .staticFuncVarArgs(constructNewTestObj, _T("constructor"), _T("*"))

        /* Using a global constructor: useful in cases where a custom
           constructor/destructor are required and the original class
           is not to be modified. */
        //.staticFunc(constructNewTestObjFixedArgs, _T("constructor"))

        /* Using a static member constructor. */
        //.staticFuncVarArgs(NewTestObj::construct, _T("constructor"))

        // Any global function can be registered in a class namespace
        // (no 'this' pointer will be passed to the function).
        .staticFunc(globalFunc, _T("globalFunc"))
        
        .staticFunc(globalClass, &GlobalClass::func, _T("globalClassFunc"))
        .func(&NewTestObj::newtestR1, _T("newtestR1"))
        .var(&NewTestObj::val, _T("val"))
        .var(&NewTestObj::s1, _T("s1"))
        .var(&NewTestObj::s2, _T("s2"))
        .var(&NewTestObj::c1, _T("c1"), VAR_ACCESS_READ_ONLY)
        .var(&NewTestObj::c2, _T("c2"), VAR_ACCESS_READ_ONLY)
        .funcVarArgs(&NewTestObj::multiArgs, _T("multiArgs"))
        ;

    BindConstant(SQ_PI * 2, _T("SQ_PI_2"));
    BindConstant(SQ_10 * 2, _T("SQ_10_2"));
    BindConstant(_T("Global String"), _T("GLOBAL_STRING"));

    SQPLUS_TEST_TRACE_SUB(testConstants0);
    RUN_SCRIPT(_T(" \n\
print(\"SQ_PI*2: \" + SQ_PI_2 + \" SQ_10_2: \" + SQ_10_2 + \" GLOBAL_STRING: \" + GLOBAL_STRING); \n\
assert(SQ_PI_2 == 6.283185307179586); \n\
assert(SQ_10_2 == 20); \n\
assert(GLOBAL_STRING == \"Global String\"); \n\
"));

    SQPLUS_TEST_TRACE_SUB(scriptedBase);
    RUN_SCRIPT(_T("\n\
class ScriptedBase { \n\
  sbval = 5551212;\n\
  function multiArgs(a, ...) { \n\
    print(\"ScriptedBase::multiArgs() \" + a + val); \n\
  } \n\
} \n\
"));                            // Note val does not exist in base.


#ifdef SQ_USE_CLASS_INHERITANCE
    /* Base class constructors, if registered, must use
       this form: static int construct(HSQUIRRELVM v). */
    SQClassDef<CustomTestObj> customClass(_T("CustomTestObj"),
                                          _T("ScriptedBase"));
    
    /* MUST use this form (or no args) if CustomTestObj will be used
       as a base class.
       Using the "*" form will allow a single constructor to be used
       for all cases. */
    customClass.staticFuncVarArgs(&CustomTestObj::construct,
                                  _T("constructor"), _T("*"));
    
    /* This form is also OK if used as a base class. */
    // customClass.staticFuncVarArgs(CustomTestObj::construct,
    //                               _T("constructor"));
    

    // "*": no type or count checking.
    customClass.funcVarArgs(&CustomTestObj::varArgTypesAndCount,
                            _T("multiArgs"), _T("*"));
    // "*": no type or count checking.    
    customClass.funcVarArgs(&CustomTestObj::varArgTypesAndCount,
                            _T("varArgTypesAndCount"), _T("*"));
    
#else
    SQClassDef<CustomTestObj> customClass(_T("CustomTestObj"));

    // string, number, bool (all types must match).      
    customClass.staticFuncVarArgs(&CustomTestObj::construct,
                                  _T("constructor"),_T("snb"));
    // "*": no type or count checking.      
    customClass.funcVarArgs(&CustomTestObj::varArgTypesAndCount,
                            _T("varArgTypesAndCount"),_T("*"));
    
#endif // SQ_USE_CLASS_INHERITANCE
      
      
    // string or number + string or number.
    customClass.funcVarArgs(&CustomTestObj::varArgTypes,
                            _T("varArgTypes"),_T("s|ns|ns|ns|n"));
    // No type string means no arguments allowed.    
    customClass.funcVarArgs(&CustomTestObj::noArgsVariableReturn,
                            _T("noArgsVariableReturn"));
    // Variables args, fixed return type.    
    customClass.func(&CustomTestObj::variableArgsFixedReturnType,
                     _T("variableArgsFixedReturnType"));
    // Many args, type checked.    
    customClass.func(&CustomTestObj::manyArgs,_T("manyArgs"));

    // Many args, type checked, one return value.    
    customClass.func(&CustomTestObj::manyArgsR1,_T("manyArgsR1"));

    
#ifdef SQ_USE_CLASS_INHERITANCE
    printf("=== BEGIN INHERITANCE ===\n");

    SQClassDef<TestBase>(_T("TestBase"))
        .var(&TestBase::x, _T("x"))
        .func(&TestBase::print, _T("print"))
        ;
    SQClassDef<TestDerivedCPP>(_T("TestDerivedCPP"))
        .func((TestDerivedCPP_print)&TestDerivedCPP::print, _T("print"))
        ;
    
    // Note that the constructor definition and call below is not
    // required for this example.
    // (The C/C++ constructor will be called automatically).

    SQPLUS_TEST_TRACE_SUB(testInheritance2);
    RUN_SCRIPT(_T("\
class TestDerived extends TestBase { \n\
  function print() {                 \n\
    ::TestBase.print();              \n\
    ::print(\"TestDerived::print() \" + x); \n\
  }                                  \n\
  constructor() {                    \n\
    TestBase.constructor();          \n\
  }                                  \n\
}                                    \n\
local a = TestDerived();             \n\
local b = TestDerived();             \n\
a.x = 1;                             \n\
b.x = 2;                             \n\
print(\"a.x = \" + a.x);             \n\
print(\"b.x = \" + b.x);             \n\
a.print();                           \n\
b.print();                           \n\
local c = TestDerivedCPP();          \n\
c.print();                           \n\
           "));

    SQPLUS_TEST_TRACE_SUB(testInheritance);
    RUN_SCRIPT(_T("\n\
class Derived extends CustomTestObj { \n\
  val = 888; \n\
  function multiArgs(a, ...) { \n\
    print(\"Derived::multiArgs() \" + a + val); \n\
    print(sbval); \n\
    ::CustomTestObj.multiArgs(4); \n\
    ::ScriptedBase.multiArgs(5, 6, 7); \n\
  } \n\
} \n\
local x = Derived(); \n\
print(\"x.val \" + x.val); \n\
x.multiArgs(1, 2, 3); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg0);
    RUN_SCRIPT(_T(" \n\
co <- CustomTestObj(\"hello\", 123, true); \n\
co.varArgTypes(\"str\", 123, 123, \"str\"); \n\
co.varArgTypes(123, \"str\", \"str\", 123); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg0a);
    RUN_SCRIPT(_T(" \n\
print(co.varArgTypesAndCount(1, true)); \n\
print(co.varArgTypesAndCount(2, false, 3.)); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg0b);
    RUN_SCRIPT(_T(" \n\
print(co.noArgsVariableReturn()); \n\
print(co.noArgsVariableReturn()); \n\
print(co.noArgsVariableReturn()); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg0c);
    RUN_SCRIPT(_T(" \n\
print(co.variableArgsFixedReturnType(1)); \n\
print(co.variableArgsFixedReturnType(1,2)); \n\
print(co.variableArgsFixedReturnType(1,2,3)); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg0d);
    RUN_SCRIPT(_T(" \n\
co.manyArgs(111, 222.2, true, \"Hello\"); \n\
print(co.manyArgsR1(333, 444.3, false, \"World\")); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg1a);
    RUN_SCRIPT(_T(" \n\
co <- CustomTestObj(\"hello\", 123, true); \n\
co.noArgsVariableReturn(); \n\
local t = NewTestObj(\"S1in\", 369, true); \n\
print(\"C1: \" + t.c1); \n\
print(\"C2: \" + t.c2); // t.c1 = 123; \n\
"));
      
    printf("===  END INHERITANCE  ===\n");
#endif // SQ_USE_CLASS_INHERITANCE


    SQPLUS_TEST_TRACE_SUB(Constant test (read only var));
    // Var can change on C++ side, but not on script side.
    RUN_SCRIPT_THROW(_T("local t = NewTestObj(); t.c1 = 123;"),
                     _T("setVar(): Cannot write to constant: c1"));
      
    SQPLUS_TEST_TRACE_SUB(testReg1);
    RUN_SCRIPT(_T(" \n\
local t = NewTestObj(); \n\
t.newtestR1(\"Hello\"); \n\
t.val = 789; \n\
print(t.val); \n\
print(t.s1); \n\
print(t.s2); \n\
t.s1 = \"New S1\"; \n\
print(t.s1); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testReg2);
    RUN_SCRIPT(_T(" \n\
local t = NewTestObj(); \n\
t.val = 789; \n\
print(t.val); \n\
t.val = 876; \n\
print(t.val); \n\
t.multiArgs(1, 2, 3); \n\
t.multiArgs(1, 2, 3, 4); \n\
t.globalFunc(\"Hola\", 5150, false); \n\
t.globalClassFunc(\"Bueno\", 5151, true); \n\
"));


    SQPLUS_TEST_TRACE_SUB(defCallFunc);
    RUN_SCRIPT(_T(" \n\
function callMe(var) { \n\
  print(\"callMe(var): I was called by: \" + var); \n\
  return 123; \n\
} \n\
"));

    SQPLUS_TEST_TRACE_SUB(defCallFuncStrRet);
    RUN_SCRIPT(_T(" \n\
function callMeStrRet(var) { \n\
  print(\"callMeStrRet(var): I was called by: \" + var); \n\
  return var; \n\
} \n\
"));


    
    SQPLUS_TEST_TRACE_SUB(SquirrelFunction);
    
    // direct function call: callMe("Squirrel 1")
    SquirrelFunction<void>(_T("callMe"))(_T("Squirrel 1"));

    // Get a function from the root table and call it.
    SquirrelFunction<int> callFunc(_T("callMe"));
    int ival = callFunc(_T("Squirrel"));
    scprintf(_T("IVal: %d\n"), ival);
    CHECK_EQUAL(ival, 123);

    SquirrelFunction<const SQChar *> callFuncStrRet(_T("callMeStrRet"));
    const SQChar *sval = callFuncStrRet(_T("Squirrel StrRet"));
    scprintf(_T("SVal: %s\n"), sval);
    CHECK_EQUAL(sval, _T("Squirrel StrRet"));

    // Argument count is checked; type is not.    
    ival = callFunc(456); 
    
    // Get a function from any table.
    SquirrelFunction<int> callFunc2(root.GetObjectHandle(), _T("callMe"));
    CHECK_EQUAL(ival, callFunc2(789));

    // === END Class Instance tests ===
}
