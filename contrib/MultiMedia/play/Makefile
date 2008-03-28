BASEDIR=$(TOP)/bin/$(AROS_HOST_ARCH)-$(AROS_HOST_CPU)
DESTDIR=$(TOP)/bin/$(AROS_TARGET_ARCH)-$(AROS_TARGET_CPU)
CC=$(BASEDIR)/tools/$(AROS_TARGET_CPU)-$(AROS_TARGET_ARCH)-aros-gcc
OBJ = ./obj/
PROJECTNAME = play
SRCS = play.c sdlfunc.c decoder.c freq.c getopt.c

OBJS = $(patsubst %.c, $(OBJ)%.o, $(SRCS))

CFLAGS = -g -O2 -I../SDL/ -Ilibavcodec -Ilibavformat -DAROS
LIBS =  -Llibavcodec -Llibavformat -L../SDL \
       -lavcodec -lavformat -lavcodec -lSDL -lasl -larossupport

all:  $(PROJECTNAME)

install:
	cp $(PROJECTNAME) $(DESTDIR)/AROS/Tests/
    
libavformat/libavformat.a: libavformat/*.c libavformat/*.h
	make -C libavformat

libavcodec/libavcodec.a: libavcodec/*.c libavcodec/*.h
	make -C libavcodec

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROJECTNAME): libavcodec/libavcodec.a libavformat/libavformat.a $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	make -C libavcodec clean 
	make -C libavformat clean 
	rm -f $(OBJ)*.o $(PROJECTNAME)

