include ../config.mk

OBJS=  Splash.o \
	SplashFontFileID.o \
	SplashBitmap.o \
	SplashClip.o \
	SplashFont.o \
	SplashFontEngine.o \
	SplashFontFile.o \
	SplashFTFont.o \
	SplashFTFontEngine.o \
	SplashFTFontFile.o \
	SplashPath.o \
	SplashPattern.o \
	SplashScreen.o \
	SplashState.o \
	SplashT1Font.o \
	SplashT1FontEngine.o \
	SplashT1FontFile.o \
	SplashXPath.o \
	SplashXPathScanner.o

TARGET = libpoppler-splash.a
INCLUDES = -I. -I.. -I../fofi -I../goo -I../../../../libs/cairo/cairo-src/MorphOS/include/cairo

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f *.o *.a

.cc.o:
	$(CC) $(CFLAGS) -O2 $(INCLUDES) -c $*.cc

.cpp.o:
	$(CXX) $(CPPFLAGS) -O2 $(INCLUDES) -c $*.cpp


