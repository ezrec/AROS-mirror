include ../config.mk

OBJS=  Annot.o \
	JPXStream.o \
	DCTStream.o \
	Array.o \
	BuiltinFont.o \
	BuiltinFontTables.o \
	Catalog.o \
	CharCodeToUnicode.o \
	CMap.o \
	Error.o \
	Dict.o \
	Decrypt.o \
	DateInfo.o \
	Gfx.o \
	Function.o \
	FontInfo.o \
	Form.o \
	FontEncodingTables.o \
	FileSpec.o \
	JArithmeticDecoder.o \
	GlobalParams.o \
	GfxState.o \
	GfxFont.o \
	Object.o \
	NameToCharCode.o \
	Movie.o \
	Link.o \
	Lexer.o \
	JBIG2Stream.o \
	Page.o \
	OutputDev.o \
	Outline.o \
	OptionalContent.o \
	PopplerCache.o \
	PDFDocEncoding.o \
	PDFDoc.o \
	Parser.o \
	PageTransition.o \
	Stream.o \
	PSTokenizer.o \
	PreScanOutputDev.o \
	ProfileData.o \
	Sound.o \
	TextOutputDev.o \
	PSOutputDev.o \
	XRef.o \
	UnicodeTypeTable.o \
	UnicodeMap.o \
	XpdfPluginAPI.o \
	SecurityHandler.o \
	PageLabelInfo.o \
	CairoOutputDev.o \
	CairoRescaleBox.o \
	CairoFontEngine.o \
	CachedFile.o \
	Rendition.o \
	ViewerPreferences.o \
	Linearization.o \
	Hints.o \
	FlateStream.o \
	SplashOutputDev.o \
	UTF.o

TARGET = libpoppler.a
INCLUDES = $(GLOBAL_INCLUDES) -I. -I.. -I../fofi -I../goo -I../../../../libs/cairo/cairo-src/MorphOS/include/cairo -I../../../../libs/jpeg6b/libjpeg

# For building with the public SDK
INCLUDES += -I ../../include -I../../include/cairo

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f *.o *.a

# build with lower optimization level. these are using isdigit() and it can miscompile (write some smarter rule for these?)
XRef.o: XRef.cc
	$(CC) $(CFLAGS) -O1 $(INCLUDES) -c $*.cc
Lexer.o: Lexer.cc
	$(CC) $(CFLAGS) -O1 $(INCLUDES) -c $*.cc
Function.o: Function.cc
	$(CC) $(CFLAGS) -O1 $(INCLUDES) -c $*.cc
GfxFont.o: GfxFont.cc
	$(CC) $(CFLAGS) -O1 $(INCLUDES) -c $*.cc

GlobalParams.o: GlobalParams.cc GlobalParamsMorphOS.cc
	$(CC) $(CFLAGS) -O2 $(INCLUDES) -c $*.cc
	
.cc.o:
	$(CC) $(CFLAGS) -O2 $(INCLUDES) -c $*.cc

.cpp.o:
	$(CXX) $(CPPFLAGS) -O2 $(INCLUDES) -c $*.cpp


