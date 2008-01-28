TARGET = win32dialog.dll
OBJS = ../tp_stub.o ../ncbind/ncbind.o win32dialog.o
CXXFLAGS += -DUNICODE -O2 -I.. -I../ncbind -DDEBUG -Wall -Wno-unused-parameter



all: $(TARGET)

$(TARGET): $(OBJS)
	dllwrap -k -def ../ncbind/ncbind.def --driver-name $(CXX) -o $@ $(OBJS)
	strip $@

win32dialog.o: win32dialog.cpp dialog.hpp dialog_config.hpp

clean:
	rm -f $(TARGET) $(OBJS)


KRKR_PLUGINS = ../../../../bin/win32/plugin/
KRKR_EXE     = ../../../../bin/win32/krkr.exe
TEST_DIR     = ../../../../tests/win32dialog
KRKR_OPT     = -debug  "`pwd -W`/$(TEST_DIR)"

test: $(TARGET)
	cp $(TARGET) $(KRKR_PLUGINS)
	$(KRKR_EXE) $(KRKR_OPT)

