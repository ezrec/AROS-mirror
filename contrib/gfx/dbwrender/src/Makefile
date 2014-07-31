all: ray display

ray: CFLAGS = -mno-cygwin -O3 -w
ray: cal.o ext.o fil.o hit.o int.o mth.o ray.o rnd.o tex.o val.o
	$(CC) -mno-cygwin -s -o $@ $^

LIBPNGDIR = ../../../Libraries/libpng

png.o: $(LIBPNGDIR)/png.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngerror.o: $(LIBPNGDIR)/pngerror.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngmem.o: $(LIBPNGDIR)/pngmem.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngset.o: $(LIBPNGDIR)/pngset.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngtrans.o: $(LIBPNGDIR)/pngtrans.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngwio.o: $(LIBPNGDIR)/pngwio.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngwrite.o: $(LIBPNGDIR)/pngwrite.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngwtran.o: $(LIBPNGDIR)/pngwtran.c
	$(CC) $(CFLAGS) -c -o $@ $^
pngwutil.o: $(LIBPNGDIR)/pngwutil.c
	$(CC) $(CFLAGS) -c -o $@ $^

ZLIBDIR = ../../../Libraries/zlib

adler32.o: $(ZLIBDIR)/adler32.c
	$(CC) $(CFLAGS) -c -o $@ $^
compress.o: $(ZLIBDIR)/compress.c
	$(CC) $(CFLAGS) -c -o $@ $^
crc32.o: $(ZLIBDIR)/crc32.c
	$(CC) $(CFLAGS) -c -o $@ $^
deflate.o: $(ZLIBDIR)/deflate.c
	$(CC) $(CFLAGS) -c -o $@ $^
inffast.o: $(ZLIBDIR)/inffast.c
	$(CC) $(CFLAGS) -c -o $@ $^
inflate.o: $(ZLIBDIR)/inflate.c
	$(CC) $(CFLAGS) -c -o $@ $^
inftrees.o: $(ZLIBDIR)/inftrees.c
	$(CC) $(CFLAGS) -c -o $@ $^
trees.o: $(ZLIBDIR)/trees.c
	$(CC) $(CFLAGS) -c -o $@ $^
zutil.o: $(ZLIBDIR)/zutil.c
	$(CC) $(CFLAGS) -c -o $@ $^

display.o: display.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $^
displayres.o: display.rc
	windres $^ $@

display: CFLAGS = -mno-cygwin -O3 -I$(LIBPNGDIR) -I$(ZLIBDIR)
display: display.o displayres.o \
	png.o pngerror.o pngmem.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o \
	adler32.o compress.o crc32.o deflate.o inffast.o inflate.o inftrees.o trees.o zutil.o
	$(CXX) -mno-cygwin -mwindows -s -o $@ $^ -mwindows -lcomctl32

clean:
	rm -f *.o

