BASEDIR=$(TOP)/bin/linux-i386$(AROS_HOST_CPU)
CC=$(BASEDIR)/tools/$(AROS_TARGET_CPU)-$(AROS_TARGET_ARCH)-aros-gcc
OBJ = ./obj/
PROJECTNAME = play
SRCS = play.c sdlfunc.c decoder.c freq.c getopt.c

OBJS = $(patsubst %.c, $(OBJ)%.o, $(SRCS))

CFLAGS = -g -O2 -I../SDL/ -Ilibavcodec -Ilibavformat
LIBS =  -Llibavcodec -Llibavformat -L../SDL \
       -lavcodec -lavformat -lavcodec -lSDL -larossupport

all:  $(PROJECTNAME)

libavformat/libavformat.a:
	cd libavformat && make && cd ..

libavcodec/libavcodec.a:
	cd libavcodec && make && cd ..

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROJECTNAME): libavcodec/libavcodec.a libavformat/libavformat.a $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJ)*.o $(PROJECTNAME)

