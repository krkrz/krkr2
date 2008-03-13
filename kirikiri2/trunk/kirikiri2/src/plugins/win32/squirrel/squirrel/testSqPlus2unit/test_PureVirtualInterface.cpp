#include "testEnv.hpp"

class PureInterface {
public:
    virtual void pureFunc1(void) = 0;
    virtual void pureFunc2(void) = 0;
};

class MyImp : public PureInterface {
public:
    PureInterface *getInterface(void) {
        return static_cast<PureInterface*>(this);
    }
    virtual void pureFunc1(void) {
        scprintf(_T("PureFunc1 called [0x%p].\n"), this);
    }
    virtual void pureFunc2(void) {
        scprintf(_T("PureFunc2 called [0x%p].\n"), this);
    }
};

class InterfaceHolder {
public:
  PureInterface *theInterface;
  void setInterface(PureInterface *pureInterface) {
      theInterface = pureInterface;
  }
  PureInterface *getInterface(void) {
      return theInterface;
  }
};

DECLARE_INSTANCE_TYPE(PureInterface);
DECLARE_INSTANCE_TYPE(MyImp);
DECLARE_INSTANCE_TYPE(InterfaceHolder);


SQPLUS_TEST(Test_PureVirtualInterface)
{
    SQPLUS_TEST_TRACE();
    
    using namespace SqPlus;
    
    SQClassDefNoConstructor<PureInterface>(_T("PureInterface"))
        .func(&PureInterface::pureFunc1, _T("pureFunc1"))
        .func(&PureInterface::pureFunc2, _T("pureFunc2"))
        ;
    SQClassDef<InterfaceHolder>(_T("InterfaceHolder"))
        .func(&InterfaceHolder::setInterface, _T("setInterface"))
        .func(&InterfaceHolder::getInterface, _T("getInterface"))
        ;
    SQClassDef<MyImp>(_T("MyImp"))
        .func(&MyImp::getInterface, _T("getInterface"))
        ;

    MyImp myImp;

    RUN_SCRIPT(_T("ih <- InterfaceHolder();"));

    SquirrelObject root = SquirrelVM::GetRootTable();
    SquirrelObject ih = root.GetValue(_T("ih"));
    InterfaceHolder *ihp = reinterpret_cast<InterfaceHolder*>(
        ih.GetInstanceUP(ClassType<InterfaceHolder>::type())
        );
    ihp->setInterface(&myImp);

    RUN_SCRIPT(_T("\
      ih.getInterface().pureFunc1(); \n\
      ih.getInterface().pureFunc2(); \n\
      ihp <- ih.getInterface(); \n\
      ihp.pureFunc1(); \n\
      ihp.pureFunc2(); \n\
      myIh <- MyImp(); \n\
      ih.setInterface(myIh.getInterface()); \n\
      ih.getInterface().pureFunc1(); \n\
      ih.getInterface().pureFunc2(); \n\
    "));
}
