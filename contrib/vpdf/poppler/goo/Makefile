include ../config.mk

OBJS=  gfile.o \
	gmempp.o   \
	GooHash.o  \
	GooList.o  \
	GooTimer.o \
	FixedPoint.o \
	ImgWriter.o  \
	PNGWriter.o  \
	JpegWriter.o \
	gstrtod.o    \
	gmem.o       \
	GooString.o  \
	grandom.o

TARGET = libgoo.a
INCLUDES = -I. -I.. -I../poppler

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f *.o *.a

# build with lower optimization level. these are using isdigit() and it can miscompile (write some smarter rule for these?)
gstrtod.o: gstrtod.cc
	$(CC) $(CFLAGS) -O1 $(INCLUDES) -c $*.cc

.cc.o:
	$(CC) $(CFLAGS) -O2 $(INCLUDES) -c $*.cc

.cpp.o:
	$(CXX) $(CPPFLAGS) -O2 $(INCLUDES) -c $*.cpp

