#include "testEnv.hpp"


SQPLUS_TEST(Test_SimpleVariable)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    SquirrelObject root = SquirrelVM::GetRootTable();

    // === BEGIN Simple variable binding tests ===
    
    int iVar = 777;
    float fVar = 88.99f;
    bool bVar = true;
    BindVariable(root, &iVar, _T("iVar"));
    BindVariable(root, &fVar, _T("fVar"));
    BindVariable(root, &bVar, _T("bVar"));

    CHECK_EQUAL(root.GetValue(_T("iVar")).ToInteger(), 777);
    CHECK_CLOSE(root.GetValue(_T("fVar")).ToFloat(), 88.99f, 1.0E-6);
    CHECK_EQUAL(root.GetValue(_T("bVar")).ToBool(), true);
    
    iVar = 999;
    CHECK_EQUAL(root.GetValue(_T("iVar")).ToInteger(), 999);
    
    fVar = 77.88f;
    CHECK_CLOSE(root.GetValue(_T("fVar")).ToFloat(), 77.88f, 1.0E-6);
    
    bVar = false;
    CHECK_EQUAL(root.GetValue(_T("bVar")).ToBool(), false);
    
    //
    static ScriptStringVar128 testString;
    scsprintf(testString, _T("This is a test string"));
    BindVariable(root, &testString, _T("testString"));
    
    CHECK_EQUAL(root.GetValue(_T("testString")).ToString(),
                _T("This is a test string"));

    scsprintf(testString, _T("This is a modified string"));
    CHECK_EQUAL(root.GetValue(_T("testString")).ToString(),
                _T("This is a modified string"));
    
    // === END Simple variable binding tests ===
}
    
