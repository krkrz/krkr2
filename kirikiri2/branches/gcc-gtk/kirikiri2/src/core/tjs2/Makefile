EXECUTABLE = ../tjscgi


CPPFILES = \
        tjs.cpp \
        tjs.tab.cpp \
        tjsArray.cpp \
        tjsCompileControl.cpp \
        tjsConfig.cpp \
        tjsDate.cpp \
        tjsDateParser.cpp \
        tjsdate.tab.cpp \
        tjsDebug.cpp \
        tjsDictionary.cpp \
        tjsDisassemble.cpp \
        tjsError.cpp \
        tjsException.cpp \
        tjsGlobalStringMap.cpp \
        tjsInterCodeExec.cpp \
        tjsInterCodeGen.cpp \
        tjsInterface.cpp \
        tjsLex.cpp \
        tjsMath.cpp \
        tjsMessage.cpp \
        tjsNamespace.cpp \
        tjsNative.cpp \
        tjsObject.cpp \
        tjsScriptBlock.cpp \
        tjsScriptCache.cpp \
        tjsString.cpp \
        tjsUtils.cpp \
        tjsVariant.cpp \
        tjsVariantString.cpp \
        tjsRandomGenerator.cpp \
        tjsMT19937ar-cok.cpp \
        tjspp.tab.cpp 
#        imdkcv/src/imdkcvlib-code.cpp \
#        imdkcv/src/imdkcvlib-jis1.cpp \
#        imdkcv/src/imdkcvlib-jis2.cpp \
#        imdkcv/src/imdkcvlib-main.cpp \
#        imdkcv/src/imdkcvlib-misc.cpp \
#        imdkcv/src/imdkcvlib-sjis.cpp \
#        main.cpp

HEADS   = \
        tjs.h \
        tjs.tab.h \
        tjsArray.h \
        tjsCommHead.h \
        tjsCompileControl.h \
        tjsConfig.h \
        tjsDate.h \
        tjsDateParser.h \
        tjsDebug.h \
        tjsDictionary.h \
        tjsError.h \
        tjsErrorDefs.h \
        tjsError_jp.h \
        tjsException.h \
        tjsGlobalStringMap.h \
        tjsHashSearch.h \
        tjsInterCodeExec.h \
        tjsInterCodeGen.h \
        tjsInterface.h \
        tjsLex.h \
        tjsMath.h \
        tjsMessage.h \
        tjsNamespace.h \
        tjsNative.h \
        tjsObject.h \
        tjsScriptBlock.h \
        tjsScriptCache.h \
        tjsString.h \
        tjsTypes.h \
        tjsUtils.h \
        tjsVariant.h \
        tjsVariantString.h \
        tjsRandomGenerator.h \
        tjsMT19937ar-cok.h \
        tjspp.tab.h 
#        imdkcv/src/imdkcv.h \
#        main.h

OBJS    = $(CPPFILES:.cpp=.o)

#INCLUDEPATH = -Itjs2 -Iimdkcv/src -I.
INCLUDEPATH = -Itjs2 -I.

#------------------- Debugging
# DEBUG   = -ggdb -pg
#DEFINES   = -DTJS_NO_REGEXP
#OPTFLAGS  = -O2 -march=pentium4 -fomit-frame-pointer -frename-registers
OPTFLAGS  = -O2

#------------------- link
LD       = gcc
LDFLAGS  = $(DEBUG)
#LDLIBS   = -lm -lstdc++ -lpq
LDLIBS   = -lm -lstdc++

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





