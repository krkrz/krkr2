CPPFLAGS =  -I../../../include -I/usr/X11R6/include
LDFLAGS = -L/usr/X11R6/lib -L../../../lib/Linux -lIrrlicht -lGL -lGLU -lXxf86vm -lXext -lX11

SRCS = CFontTool.cpp main.cpp
OBJS = $(SRCS:%.cpp=%.o)
DEST = FontTool

all: $(DEST)

FontTool: $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(DEST)
