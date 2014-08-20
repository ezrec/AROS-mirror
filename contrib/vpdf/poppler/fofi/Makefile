include ../config.mk

OBJS= FoFiBase.o \
	FoFiEncodings.o \
	FoFiTrueType.o \
	FoFiType1C.o \
	FoFiType1.o

TARGET = libfofi.a
INCLUDES = -I. -I..

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f *.o *.a

.cc.o:
	$(CC) $(CFLAGS) -O2 $(INCLUDES) -c $*.cc

.cpp.o:
	$(CXX) $(CPPFLAGS) -O2 $(INCLUDES) -c $*.cpp


