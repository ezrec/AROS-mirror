BASEDIR=$(TOP)/bin/$(AROS_TARGET_ARCH)-$(AROS_TARGET_CPU)
CC=$(BASEDIR)/tools/$(AROS_TARGET_CPU)-$(AROS_TARGET_ARCH)-aros-gcc
OBJ = ./obj/
PROJECTNAME = play
SRCS = play.c sdlfunc.c decoder.c freq.c getopt.c 

OBJS = $(patsubst %.c, $(OBJ)%.o, $(SRCS))

CFLAGS = -g -O2 -I../SDL/ -Ilibavcodec -Ilibavformat
LIBS = -nostdlib -nostartfiles -s -Llibavcodec -Llibavformat -L../SDL \
       -lavcodec -lavformat -lavcodec -lSDL -lamiga -larosc -larosm \
       -lautoinit -larossupport -larosc
       
STARTUP = $(BASEDIR)/AROS/lib/startup.o

GCCLIB = `$(CC) --print-libgcc-file`

all: $(PROJECTNAME) 

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROJECTNAME): $(OBJS)
	$(CC) -o $@ $(STARTUP) $(OBJS) $(LIBS) $(GCCLIB)

clean: 
	rm -f $(OBJ)*.o $(PROJECTNAME)

