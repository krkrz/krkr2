#ifndef NEWTESTOBJ_HPP
#define NEWTESTOBJ_HPP

struct NewTestObj {
    SqPlus::ScriptStringVar64 s1;
    SqPlus::ScriptStringVar32 s2;
    bool b;
    int val;
    int c1;
    SqPlus::ScriptStringVar8 c2; // 8 char plus null (max string is 8
                                 // printable chars).
    NewTestObj() : val(777) {
        s1 = _T("s1 = s1");
        s2 = _T("s2 = s2");
        c1 = 996;
        c2 = _T("It's a 997"); // Prints: "It's a 9", as only 8 chars
                               // in static buffer (plus null).
    }

    NewTestObj(const SQChar *_s1, int _val, bool _b) {
        s1  = _s1;
        val = _val;
        b   = _b;
        s2 = _T("s2 = s2");
        c1 = 993;
        c2 = _T("It's a 998"); // Prints: "It's a 9", as only 8 chars
                               // in static buffer (plus null).
    }

    static int construct(HSQUIRRELVM v) {
        using namespace SqPlus;
        // StackHandler sa(v);
        // SquirrelObject so = sa.GetObjectHandle(1);
        return PostConstruct<NewTestObj>(v, new NewTestObj(), release);
    } // construct

    SQ_DECLARE_RELEASE(NewTestObj) // Required when using a custom constructor.
        void newtest(void) {
        scprintf(_T("NewTest: %d\n"), val);
    }
    SQChar *newtestR1(const SQChar *inString) {
        scprintf(_T("NewTestR1: Member var val is %d, function arg is %s\n"),
                 val, inString);
        return _T("Returned String from NewTestObj::newtestR1");
    }

    int multiArgs(HSQUIRRELVM v) {
        StackHandler sa(v);
        SquirrelObject so = sa.GetObjectHandle(1);
        int paramCount = sa.GetParamCount();
        int p1 = sa.GetInt(2);
        int p2 = sa.GetInt(3);
        int p3 = sa.GetInt(4);
        return 0;
    } // multiArgs

    int _set(HSQUIRRELVM v) {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        const SQChar *el = sa.GetString(2);
        val = sa.GetInt(3);
        return sa.Return(val);
    }

    int _get(HSQUIRRELVM v) {
        StackHandler sa(v);
        int paramCount = sa.GetParamCount();
        return sa.Return(val);
    }
};

#endif // NEWTESTOBJ_HPP
