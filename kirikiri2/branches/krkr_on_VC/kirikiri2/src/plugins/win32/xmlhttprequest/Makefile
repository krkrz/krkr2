TARGET=xhr.dll
OBJS=Main.o ../tp_stub.o
BOOST_LIB_DIR=/usr/local/src/boost_1_33_1/stage/lib
BOOST_INCLUDE_DIR=/usr/local/src/boost_1_33_1
CXXFLAGS=-D_MT -Wall -mno-cygwin -O2 -I$(BOOST_INCLUDE_DIR) -I..
LIBS=-lboost_regex-gcc-mt-1_33_1 -lws2_32

$(TARGET): $(OBJS)
	dllwrap --add-stdcall-alias --driver-name g++ -mno-cygwin --target=i386-mingw32 -L$(BOOST_LIB_DIR) \
		-o $@ $(OBJS) $(LIBS)
	strip $@

.PHONY: clean

clean :
	rm -f $(TARGET) $(OBJS)


