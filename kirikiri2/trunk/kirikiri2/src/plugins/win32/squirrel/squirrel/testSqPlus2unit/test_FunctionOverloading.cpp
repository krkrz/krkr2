#define SQPLUS_OVERLOAD_OPT
#define SQPLUS_CONST_OPT

#include "testEnv.hpp"


/*
  Overloaded functions
 */
void overloadedFunc(void){scprintf(_T("void overloadedFunc(void)\n"));}
int overloadedFunc(int)  {scprintf(_T("int overloadedFunc(int)\n")); return 1;}
void overloadedFunc(float){scprintf(_T("void overloadedFunc(float)\n"));}

/*
  `Tuple3' class with overloaded member functions
 */
struct Tuple3 {
    float x, y, z;

    // constructors
    Tuple3(void) {x = 1.0f; y = 2.0f; z = 3.0f;}
    Tuple3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // destructor
    virtual ~Tuple3() {scprintf(_T("~Tuple3()\n"));}

    // overloaded member functions
    void set(const Tuple3 &src) {x = src.x; y = src.y; z = src.z;}
    float set(float xyz) {x = y = z = xyz; return xyz;}
    void set(float x_, float y_, float z_) {x = x_; y = y_; z = z_;}

    /* overloaded static member functions */
    static float staticVar;
    static void setStaticVar(void) {staticVar = 0.0;}
    static void setStaticVar(float f) {staticVar = f;}
};
DECLARE_INSTANCE_TYPE(Tuple3);

/* static */ float Tuple3::staticVar;


struct Point3 : public Tuple3 {
    Point3(void) : Tuple3() {}
    Point3(float _x, float _y, float _z) : Tuple3(_x, _y, _z) {}
    virtual ~Point3() {scprintf(_T("~Point3()\n"));}
    float getX(void) {return this->x;}
    float getX(float offset) {return offset + this->x;}
    void set(void) {Tuple3::set(0.0, 0.0, 0.0);}
};
DECLARE_INSTANCE_TYPE(Point3);


SQPLUS_TEST(Test_FunctionOverloading)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;

#ifdef SQPLUS_OVERLOAD_OPT
    OverloadGlobal<void(*)(void)>(&overloadedFunc, _T("overloadedFunc"));
    OverloadGlobal<int(*)(int)>(&overloadedFunc, _T("overloadedFunc"));
    OverloadGlobal<void(*)(float)>(&overloadedFunc, _T("overloadedFunc"));
    
    SQPLUS_TEST_TRACE_SUB(overload_global_functions);
    RUN_SCRIPT(_T("\
overloadedFunc(); \n\
overloadedFunc(100); \n\
overloadedFunc(2.0); \n\
"));
    
    RUN_SCRIPT_THROW(
        _T("print(\"An errorr is expected at the next line:\"); \n\
            overloadedFunc(\"foo\");"),
        _T("No match for given arguments")
        );

    
    SQPLUS_TEST_TRACE_SUB(overload_member_functions_and_constructors);
    SQClassDef<Tuple3>(_T("Tuple3"))
        // bind member variables
        .var(&Tuple3::x, _T("x"))
        .var(&Tuple3::y, _T("y"))
        .var(&Tuple3::z, _T("z"))

        // overload constructors
        .overloadConstructor<Tuple3(*)(void)>()
        .overloadConstructor<Tuple3(*)(float, float, float)>()
        .overloadConstructor<Tuple3(*)(Tuple3&)>()

        // overload member functions
        .overloadFunc<void(Tuple3::*)(const Tuple3&)>(&Tuple3::set, _T("set"))
        .overloadFunc<float(Tuple3::*)(float)>(&Tuple3::set, _T("set"))
        .overloadFunc<void(Tuple3::*)(float, float, float)>(&Tuple3::set,
                                                            _T("set"))
        // overload static member functions
        .overloadStaticFunc<void(*)(void)>(&Tuple3::setStaticVar,
                                           _T("setStaticVar"))
        .overloadStaticFunc<void(*)(float)>(&Tuple3::setStaticVar,
                                            _T("setStaticVar"))
        
        .staticVar(&Tuple3::staticVar, _T("staticVar"))
        ;


    RUN_SCRIPT(_T("\
local t3v = Tuple3(); \n\
assert(t3v.x == 1.0); \n\
assert(t3v.y == 2.0); \n\
assert(t3v.z == 3.0); \n\
\n\
local t3f = Tuple3(-11.0, 22.0, -33.0); \n\
assert(t3f.x == -11.0); \n\
assert(t3f.y == 22.0); \n\
assert(t3f.z == -33.0); \n\
\n\
local t3c = Tuple3(t3f); \n\
assert(t3c.x == t3f.x); \n\
assert(t3c.y == t3f.y); \n\
assert(t3c.z == t3f.z); \n\
\n\
t3v.set(333.0); \n\
assert(t3v.x == 333.0); \n\
assert(t3v.y == 333.0); \n\
assert(t3v.z == 333.0); \n\
\n\
t3v.set(444.0, 555.0, 666.0); \n\
assert(t3v.x == 444.0); \n\
assert(t3v.y == 555.0); \n\
assert(t3v.z == 666.0); \n\
\n\
t3v.set(t3f); \n\
assert(t3v.x == t3f.x); \n\
assert(t3v.y == t3f.y); \n\
assert(t3v.z == t3f.z); \n\
\n\
t3v.setStaticVar(); \n\
assert(t3c.staticVar == 0.0); \n\
\n\
t3f.setStaticVar(2.0); \n\
assert(t3f.staticVar == 2.0); \n\
assert(t3c.staticVar == 2.0); \n\
"));
    


# ifdef SQ_USE_CLASS_INHERITANCE
    SQClassDef<Point3>(_T("Point3"), _T("Tuple3"))
        .overloadConstructor<Point3(*)(void)>()
        .overloadConstructor<Point3(*)(float, float, float)>()
        .overloadFunc<float (Point3::*)(void)>(&Point3::getX, _T("getX"))
        .overloadFunc<float (Point3::*)(float)>(&Point3::getX, _T("getX"))
        .overloadFunc<void (Point3::*)(void)>(&Point3::set, _T("set"))
        ;

    RUN_SCRIPT(_T("\
local p3v = Point3(); \n\
assert(p3v.x == 1.0); \n\
assert(p3v.y == 2.0); \n\
assert(p3v.z == 3.0); \n\
\n\
assert(p3v.getX() == 1.0); \n\
assert(p3v.getX(-2.0) == -1.0); \n\
\n\
p3v.set(); \n\
assert(p3v.x == 0.0); \n\
assert(p3v.y == 0.0); \n\
assert(p3v.z == 0.0); \n\
"));

    // Point3::set() shadows Tuple3::set
    RUN_SCRIPT_THROW(
        _T("print(\"An errorr is expected at the next line:\"); \n\
            local p3v = Point3(); p3v.set(p3v);"),
        _T("No match for given arguments")
        );
# endif // SQ_USE_CLASS_INHERITANCE
    
#else  //SQPLUS_OVERLOAD_OPT
    
#endif //SQPLUS_OVERLOAD_OPT
}

