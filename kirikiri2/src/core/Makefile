EXECUTABLE = krkr


CPPFILES = \
        tjs2/tjs.cpp \
        tjs2/tjs.tab.cpp \
        tjs2/tjsArray.cpp \
        tjs2/tjsCompileControl.cpp \
        tjs2/tjsConfig.cpp \
        tjs2/tjsDate.cpp \
        tjs2/tjsDateParser.cpp \
        tjs2/tjsdate.tab.cpp \
        tjs2/tjsDebug.cpp \
        tjs2/tjsDictionary.cpp \
        tjs2/tjsDisassemble.cpp \
        tjs2/tjsError.cpp \
        tjs2/tjsException.cpp \
        tjs2/tjsGlobalStringMap.cpp \
        tjs2/tjsInterCodeExec.cpp \
        tjs2/tjsInterCodeGen.cpp \
        tjs2/tjsInterface.cpp \
        tjs2/tjsLex.cpp \
        tjs2/tjsMath.cpp \
        tjs2/tjsMessage.cpp \
        tjs2/tjsNamespace.cpp \
        tjs2/tjsNative.cpp \
        tjs2/tjsObject.cpp \
        tjs2/tjsScriptBlock.cpp \
        tjs2/tjsScriptCache.cpp \
        tjs2/tjsString.cpp \
        tjs2/tjsUtils.cpp \
        tjs2/tjsVariant.cpp \
        tjs2/tjsVariantString.cpp \
        tjs2/tjsRandomGenerator.cpp \
        tjs2/tjsMT19937ar-cok.cpp \
        tjs2/tjspp.tab.cpp \
	utils/DebugIntf.cpp \
	utils/ThreadIntf.cpp \
	environ/gtk/DetectCPU.cpp \
        environ/gtk/tvpgtk.cpp \
#	base/gtk/SysInitImpl.cpp \
	base/SysInitIntf.cpp \
#	base/ScriptMgnIntf.cpp

HEADS   = \
        tjs2/tjs.h \
        tjs2/tjs.tab.h \
        tjs2/tjsArray.h \
        tjs2/tjsCommHead.h \
        tjs2/tjsCompileControl.h \
        tjs2/tjsConfig.h \
        tjs2/tjsDate.h \
        tjs2/tjsDateParser.h \
        tjs2/tjsDebug.h \
        tjs2/tjsDictionary.h \
        tjs2/tjsError.h \
        tjs2/tjsErrorDefs.h \
        tjs2/tjsError_jp.h \
        tjs2/tjsException.h \
        tjs2/tjsGlobalStringMap.h \
        tjs2/tjsHashSearch.h \
        tjs2/tjsInterCodeExec.h \
        tjs2/tjsInterCodeGen.h \
        tjs2/tjsInterface.h \
        tjs2/tjsLex.h \
        tjs2/tjsMath.h \
        tjs2/tjsMessage.h \
        tjs2/tjsNamespace.h \
        tjs2/tjsNative.h \
        tjs2/tjsObject.h \
        tjs2/tjsScriptBlock.h \
        tjs2/tjsScriptCache.h \
        tjs2/tjsString.h \
        tjs2/tjsTypes.h \
        tjs2/tjsUtils.h \
        tjs2/tjsVariant.h \
        tjs2/tjsVariantString.h \
        tjs2/tjsRandomGenerator.h \
        tjs2/tjsMT19937ar-cok.h \
        tjs2/tjspp.tab.h \
        base/ScriptMgnIntf.h \
        base/gtk/ScriptMgnImpl.h \
	utils/DebugIntf.h \
	utils/ThreadIntf.h
#	base/gtk/SysInitImpl.h

OBJS    = $(CPPFILES:.cpp=.o)

#INCLUDEPATH = -Itjs2 -Iimdkcv/src -I.
INCLUDEPATH = -Itjs2 -Ivisual -Imsg -Ibase -Iutils -I. `pkg-config --cflags gtk+-2.0`

#------------------- Debugging
# DEBUG   = -ggdb -pg
#DEFINES   = -DTJS_NO_REGEXP
#OPTFLAGS  = -O2 -march=pentium4 -fomit-frame-pointer -frename-registers
OPTFLAGS  = -O2

#------------------- link
LD       = gcc
LDFLAGS  = $(DEBUG)
#LDLIBS   = -lm -lstdc++ -lpq
LDLIBS   = -lm -lstdc++ `pkg-config --libs gtk+-2.0`

#------------------- C (*.c)
CC       = gcc
CFLAGS   = $(OPTFLAGS) -Wall $(DEBUG) $(DEFINES)
CPPFLAGS = $(INCLUDEPATH)

#------------------- C++ (*.cc)
CXX      = g++
CXXFLAGS = $(OPTFLAGS) -Wall $(DEBUG) $(DEFINES)

#------------------- compiling rules
.SUFFIXES:
.SUFFIXES: .o .c .cpp .cc .f .p

build: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(OBJS): $(HEADS) # Makefile


.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@


#------------------- utils
clean:
	$(RM) $(PACKAGE) $(OBJS)
	$(RM) core gmon.out *~ #*#





