BASEDIR=$(TOP)/bin/$(AROS_TARGET_ARCH)-$(AROS_HOST_CPU)
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
	cp $(PROJECTNAME) $(BASEDIR)/AROS/Tests/
    
libavformat/libavformat.a:
	cd libavformat && make && cd ..

libavcodec/libavcodec.a:
	cd libavcodec && make && cd ..

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROJECTNAME): libavcodec/libavcodec.a libavformat/libavformat.a $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	cd libavcodec && make clean && cd ..
	cd libavformat && make clean && cd ..
	rm -f $(OBJ)*.o $(PROJECTNAME)

