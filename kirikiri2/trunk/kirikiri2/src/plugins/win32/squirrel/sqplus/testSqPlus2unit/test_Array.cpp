#include "testEnv.hpp"

/*
  from http://squirrel-lang.org/forums/post/228.aspx
*/


SQPLUS_TEST(Test_Array)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    SquirrelObject root = SquirrelVM::GetRootTable();
    
    // === BEGIN Array Tests ===

    SquirrelObject array = SquirrelVM::CreateArray(10);
    for (int i = 0; i < 10; i++) {
        array.SetValue(i, i);
    }
    array.ArrayAppend(123);  // int
    array.ArrayAppend(true); // bool (must use bool and not
                             // SQBool. SQBool is treated as INT by
                             // compiler).
    array.ArrayAppend(false); // bool (must use bool and not
                              // SQBool. SQBool is treated as INT by
                              // compiler).
    array.ArrayAppend(123.456f);     // float
    array.ArrayAppend(_T("string")); // string
    array.ArrayAppend(456);          // Will be popped and thrown away (below).
    array.ArrayAppend((SQUserPointer)0);

    
    // Pop 4 items from array:
    array.ArrayPop(SQFalse); // Don't retrieve the popped value (int:123).

    SQUserPointer up = array.GetUserPointer(0);
    array.ArrayPop();
    CHECK_EQUAL(up, (SQUserPointer)0);
    SquirrelObject so1 = array.ArrayPop();   // Retrieve the popped value.
    const SQChar *val1 = so1.ToString();    // Get string.
    const float val2 = array.ArrayPop().ToFloat(); // Pop and get float.
    scprintf(_T("[Popped values] Val1: %s, Val2: %f\n"), val1, val2);
    CHECK_EQUAL(val1, _T("string"));
    CHECK_CLOSE(val2, 123.456f, 1.0E-6);

    const int startIndex = array.Len();
    CHECK_EQUAL(array.Len(), 13);
    array.ArrayExtend(10); // Implemented as: ArrayResize(Len()+amount).
    for (int i = startIndex; i < array.Len(); i++) {
        array.SetValue(i, i*10);
    }
    CHECK_EQUAL(array.Len(), 23);
    root.SetValue(_T("array"), array);

    // Get a copy as opposed to another reference.
    SquirrelObject arrayr = array.Clone();
    arrayr.ArrayReverse();
    root.SetValue(_T("arrayr"), arrayr);
    CHECK_EQUAL(arrayr.Len(), 23);

    
    SQPLUS_TEST_TRACE_SUB(define_printArray);
    RUN_SCRIPT(_T("\n\
function printArray(name,array) { \n\
  print(name + \".len() = \" + array.len()); \n\
  foreach(i, v in array) if (v != null) {\n\
    if (typeof v == \"bool\") v = v ? \"true\" : \"false\";\n\
    print(\"[\"+i+\"]: \"+v); \n\
  } \n\
}"));

    SQPLUS_TEST_TRACE_SUB(test);
    RUN_SCRIPT(_T("\n\
printArray(\"array\", array); \n\
assert(array.len() == 23); \n\
assert(!array[12]); \n\
assert(array[16] == 160); \n\
printArray(\"arrayr\", arrayr); \n\
assert(arrayr.len() == 23); \n\
assert(!arrayr[10]); \n\
assert(arrayr[16] == 6); \n\
"));
}
