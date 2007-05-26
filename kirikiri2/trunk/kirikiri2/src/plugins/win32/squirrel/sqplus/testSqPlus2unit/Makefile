SQUIRREL= ..

UNITTEST=UnitTest++

OUT= ./testSqPlus2unit
INCDIRS= -I$(SQUIRREL)/include -I. -I$(SQUIRREL)/sqlibs -I$(SQUIRREL)/sqplus \
	-I$(UNITTEST)/src
LIBDIR= -L$(SQUIRREL)/lib -L$(UNITTEST)
LIBS= -lsqplus -lsquirrel -lsqstdlib -lUnitTest++


.SUFFIXES: .o .cpp

SRCS= \
	main.cpp \
	globals.cpp \
	test_Array.cpp \
	test_ClassInstance.cpp \
	test_FunctionOverloading.cpp \
	test_GlobalFunctionBindings.cpp \
	test_Instance.cpp \
	test_Namespace.cpp \
	test_ObjectGetSet.cpp \
	test_PassByReference.cpp \
	test_PointfBoxf.cpp \
	test_PureVirtualInterface.cpp \
	test_ScriptingTypeName.cpp \
	test_Scripts.cpp \
	test_SimpleVariable.cpp \
	test_Table.cpp \
	testEnv.cpp

OBJS = $(patsubst %.cpp, %.o, $(src))

all: testSqPlus2unit

FORCE: ;

testSqPlus2unit: FORCE 
	cd $(UNITTEST) && make
	g++ -g -fno-rtti -o $(OUT) $(SRCS) $(INCDIRS) $(LIBDIR) $(LIBS)
