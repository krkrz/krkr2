KIRIKIRI_DIR = ../../kr2_228/kirikiri2
KIRIKIRI_OPT = -nosel -debug -forcelog=clear -logerror=clear
KIRIKIRI = $(KIRIKIRI_DIR)/krkr.exe 

TARGET = testbind.dll
OBJS = ../tp_stub.o ncbind.o testbind.o
CXXFLAGS += -O2 -I.. -DDEBUG



all: $(TARGET)

$(TARGET): $(OBJS)
	dllwrap -k -def ncbind.def --driver-name $(CXX) -o $@ $(OBJS)
	strip $@

ncbind.hpp: ncb_invoke.hpp ncb_foreach.h
ncbind.o:   ncbind.cpp ncbind.hpp
testbind.o: testbind.cpp ncbind.hpp

clean:
	rm -f $(TARGET) $(OBJS)

test:
	cp $(TARGET) $(KIRIKIRI_DIR)
	$(KIRIKIRI) $(KIRIKIRI_OPT)


