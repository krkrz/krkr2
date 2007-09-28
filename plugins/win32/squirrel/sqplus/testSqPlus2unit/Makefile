OUT= testSqPlus2unit

all: $(OUT)

SQUIRREL= ..

CXX	= g++
CXXFLAGS= -g -O0 -fno-rtti -Wall
INCDIRS= -I$(SQUIRREL)/include -I. -I$(SQUIRREL)/sqlibs -I$(SQUIRREL)/sqplus \
	-I$(UNITTEST)/src
LIBDIR= -L$(SQUIRREL)/lib -L$(UNITTEST)
LIBS= -lsqplus -lsquirrel -lsqstdlib -lUnitTest++

UNITTEST=UnitTest++

.SUFFIXES: .o .cpp

SRCS= \
	main.cpp \
	globals.cpp \
	test_Array.cpp \
	test_ClassInstance.cpp \
	test_FunctionOverloading.cpp \
	test_GlobalFunctionBindings.cpp \
	test_GlobalAsMember.cpp \
	test_Instance.cpp \
	test_MemberVariableTypes.cpp \
	test_Namespace.cpp \
	test_ObjectGetSet.cpp \
	test_PassByReference.cpp \
	test_PointfBoxf.cpp \
	test_PointerToDerived.cpp \
	test_PureVirtualInterface.cpp \
	test_ScriptingTypeName.cpp \
	test_Scripts.cpp \
	test_SimpleVariable.cpp \
	test_SmartPointer.cpp \
	test_Table.cpp \
	testEnv.cpp



OBJS = $(patsubst %.cpp, %.o, $(SRCS))


.cpp.o:
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $<

testSqPlus2unit: $(OBJS)
	cd $(UNITTEST) && make
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJS) $(LIBDIR) $(LIBS)
