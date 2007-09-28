#ifndef GLOBALS_HPP
#define GLOBALS_HPP

class GlobalClass {
public:
    void func(const SQChar *s,int val) {
        scprintf(_T("GlobalClass::func(s, val): s: %s val: %d\n"),s,val);
    } // func
};
extern GlobalClass globalClass;
extern int globalVar;

void globalFunc(const SQChar *s, int val);

#endif // GLOBALS_HPP
