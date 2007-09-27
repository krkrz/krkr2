#include "testEnv.hpp"
#include "globals.hpp"
#include "constants.hpp"

static const int intConstant     = 7;
static const float floatConstant = 8.765f;
static const bool boolConstant   = true;

struct Vector3 {
    static float staticVar;
    float x, y, z;
    Vector3() {
        x = 1.f;
        y = 2.f;
        z = 3.f;
    }
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    ~Vector3() {
        scprintf(_T("~Vector()\n"));
    }
    Vector3 Inc(Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    } // Inc
    Vector3 operator+(Vector3 & v) {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
};
float Vector3::staticVar = 898.434f;
DECLARE_INSTANCE_TYPE(Vector3);

Vector3
Add2(Vector3 &a,Vector3 &b)
{
    Vector3 c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
} // Add2

int
Add(HSQUIRRELVM v)
{
    using namespace SqPlus;
    Vector3 *self = GetInstance<Vector3,true>(v, 1);
    Vector3 *arg  = GetInstance<Vector3,true>(v, 2);
#if 0
    SQUserPointer type = 0;
    so.GetTypeTag(&type);
    SQUserPointer reqType = ClassType<Vector3>::type();
    if (type != reqType) {
        throw SquirrelError(_T("Invalid class type"));
    } // if
#endif
    // Vector3 *self = (Vector3 *)so.GetInstanceUP(ClassType<Vector3>::type());
    // if (!self) throw SquirrelError(_T("Invalid class type"));
    Vector3 tv;
    tv.x = arg->x + self->x;
    tv.y = arg->y + self->y;
    tv.z = arg->z + self->z;
    return ReturnCopy(v, tv);
}

enum {SQ_ENUM_TEST = 1234321};


SQPLUS_TEST(Test_ObjectGetSet)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    
    SQClassDef<Vector3>(_T("Vector3"))
        .var(&Vector3::x, _T("x"))
        .var(&Vector3::y, _T("y"))
        .var(&Vector3::z, _T("z"))
        .func(&Vector3::Inc, _T("Inc"))
        .func(&Vector3::operator+, _T("_add"))
        .staticFunc(&Add2, _T("Add2"))
        .staticFuncVarArgs(&Add, _T("Add"))
        .staticVar(&Vector3::staticVar,_T("staticVar"), VAR_ACCESS_READ_ONLY)
        .staticVar(&globalVar, _T("globalVar"))
        .constant(SQ_10, _T("SQ_10"))
        .constant(SQ_E, _T("SQ_E"))
        .constant(SQ_PI, _T("SQ_PI"))
        .constant(SQ_CONST_STRING, _T("SQ_CONST_STRING"))
        .enumInt(SQ_ENUM_TEST, _T("SQ_ENUM_TEST"))
        .constant(intConstant, _T("intConstant"))
        .constant(floatConstant, _T("floatConstant"))
        .constant(true, _T("boolTrue"))
        .constant(false, _T("boolFalse"))
        .constant(boolConstant, _T("boolConstant"))
        ;

    /*    
          We can pass in arguments:
          
          by value (required for constant float, int, etc., or when a
                    copy is desired),
  
          by reference (data will be copied to SquirrelObject and
                        memory managed),
  
          by pointer (no data copying: pointer is used directly in
                      SquirrelObject; the memory will not be managed).
    */

    // constant argument is passed by value
    // (even though declaration is by ref: const & results in
    // by-value in this case), memory will be allocated and
    // managed for the copy.
    SquirrelObject tc(5.678f); 
    float valc = tc.Get<float>();
    scprintf(_T("Valc is: %f\n"), valc);
    CHECK_EQUAL(valc, 5.678f);

    float val = 1.234f;
    SquirrelObject t(val); // val is passed by reference, memory will
                           // be allocated, and the value copied once.

    float val2 = t.Get<float>();
    scprintf(_T("Val2 is: %f\n"), val2);
    CHECK_EQUAL(val2, 1.234f);

    if (1) {
        // Pass in by reference: will be copied once, with memory for
        // new copy managed by Squirrel.
        SquirrelObject v(Vector3(1.f, 2.f, 3.f));

        Vector3 * pv = v.Get<Vector3 *>();
        scprintf(_T("Vector3 is: %f %f %f\n"), pv->x, pv->y, pv->z);
        CHECK_EQUAL(pv->x, 1.f);
        CHECK_EQUAL(pv->y, 2.f);
        CHECK_EQUAL(pv->z, 3.f);
        pv->z += 1.f;

        if (1) {
            // This is a pointer to v's instance (passed in by
            // pointer: see SquirrelObject.h).
            SquirrelObject v2p(pv);
            
            // A new Squirrel Instance will be created, but the C++
            // instance pointer will not get freed when v2p goes out
            // of scope (release hook will be null).
            pv = v2p.Get<Vector3 *>();
            scprintf(_T("Vector3 is: %f %f %f\n"), pv->x, pv->y, pv->z);
            CHECK_EQUAL(pv->x, 1.f);
            CHECK_EQUAL(pv->y, 2.f);
            CHECK_EQUAL(pv->z, 4.f);
        } // if
    } // if
    
    scprintf(_T("Vector3() instance has been released.\n\n"));

    
    SQPLUS_TEST_TRACE_SUB(testStaticVars);
    RUN_SCRIPT(_T(" \n\
local v = Vector3(); \n\
local v2 = Vector3(); \n\
local v3 = v + v2; \n\
v3 += v2; \n\
print(\"v3.x: \"+v3.x); \n\
print(\"Vector3::staticVar: \" + v.staticVar); \n\
assert(v.staticVar == 898.434); \n\
print(\"Vector3::globalVar: \" + v.globalVar); \n\
assert(v.globalVar == 5551234); \n\
"));
    SQPLUS_TEST_TRACE_SUB(testStaticVarsConstant);
    RUN_SCRIPT_THROW(_T("local v = Vector3(); v.staticVar = 0;"),
                     _T("setVar(): Cannot write to constant: staticVar"));

    
    SQPLUS_TEST_TRACE_SUB(testConstants1);
    RUN_SCRIPT(_T("\
local v = Vector3(); \n\
print(\"SQ_10: \" + v.SQ_10) \n\
assert(v.SQ_10 == 10); \n\
print(\"SQ_E: \" + v.SQ_E) \n\
assert(v.SQ_E - 2.71828182845904523536 < 1.0E-6); \n\
print(\"SQ_PI: \" + v.SQ_PI) \n\
assert(v.SQ_PI - 3.14159265358979323846 < 1.0E-6); \n\
print(\"SQ_CONST_STRING: \" + v.SQ_CONST_STRING) \n\
assert(v.SQ_CONST_STRING == \"A constant string\"); \n\
print(\"SQ_ENUM_TEST: \" + v.SQ_ENUM_TEST); \n\
assert(v.SQ_ENUM_TEST == 1234321); \n\
" ));

    
    SQPLUS_TEST_TRACE_SUB(testConstants2);
    RUN_SCRIPT(_T("\
local v = Vector3(); \n\
print(\"intConstant: \" + v.intConstant); \n\
assert(v.intConstant == 7); \n\
print(\"floatConstant: \" + v.floatConstant); \n\
assert(v.floatConstant == 8.765); \n\
print(\"boolTrue: \" + (v.boolTrue ? \"True\" : \"False\" )); \n\
assert(v.boolTrue); \n\
print(\"boolFalse: \" + (v.boolFalse ? \"True\" : \"False\" )); \n\
assert(!v.boolFalse); \n\
print(\"boolConstant: \" + (v.boolConstant ? \"True\" : \"False\"));\n\
assert(v.boolConstant); \n\
"));

    SQPLUS_TEST_TRACE_SUB(testRegV);
    RUN_SCRIPT(_T("\
local vec = Vector3(); \n\
print(vec.x); \n\
assert(vec.x == 1.); \n\
vec = vec.Add(vec); \n\
print(vec.x); \n\
assert(vec.x == 2.); \n\
vec = vec.Add(vec); \n\
print(vec.x); \n\
assert(vec.x == 4.); \n\
vec = vec.Add2(vec, vec); \n\
print(vec.x); \n\
assert(vec.x == 8.); \n\
local v2 = Vector3(); \n\
vec = v2.Inc(vec); \n\
print(vec.x); \n\
assert(vec.x == 9.); \n\
print(v2.x); \n\
assert(v2.x == 9.); \n\
"));
}
