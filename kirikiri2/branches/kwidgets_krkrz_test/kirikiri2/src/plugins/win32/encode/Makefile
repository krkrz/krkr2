TARGET=encode.dll
OBJS=encode.o ../tp_stub.o
CXXFLAGS=-Wall -mno-cygwin -O2 -I.. --input-charset=cp932

$(TARGET): $(OBJS)
	dllwrap --add-stdcall-alias --driver-name g++ -mno-cygwin --target=i386-mingw32 -o $@ $(OBJS)
	strip $@

.PHONY: clean

clean :
	rm -f $(TARGET) $(OBJS)

encode.o: e2u.h hojo2u.h u2e.h
