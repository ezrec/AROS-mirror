all: game

CC = gcc
XLIB = -L/usr/X11R6/lib -I/usr/X11R6/include
LIBS = -lm -lX11
OBJS = game.o aliens.o player.o effects.o \
object.o camera.o main-x11.o vec4x1.o mat4x4.o
DEPENDS = 
#CFLAGS = -g -Wall -pedantic -ansi
CFLAGS = -ansi
#OPTIONS = -D_GNU_SOURCE -DGAME_LINUX_X11 -DGAME_DEBUG
OPTIONS= -DGAME_LINUX_X11

.c.o: $(DEPENDS)
	$(CC) -c $(CFLAGS) $(OPTIONS) $< -o $@

game: $(OBJS) $(DEPENDS)
	$(CC) -o xinv3d $(OBJS) $(XLIB) $(LIBS) $(OPTIONS)

clean:
	rm *.o
	rm xinv3d

aliens.o: aliens.c game.h vec4x1.h mat4x4.h camera.h system.h defines.h \
 object.h externs.h
camera.o: camera.c mat4x4.h vec4x1.h camera.h
effects.o: effects.c game.h vec4x1.h mat4x4.h camera.h system.h \
 defines.h object.h externs.h
game.o: game.c game.h vec4x1.h mat4x4.h camera.h system.h defines.h \
 object.h externs.h
object.o: object.c mat4x4.h vec4x1.h defines.h object.h
main-x11.o: main-x11.c game.h vec4x1.h mat4x4.h camera.h system.h \
 defines.h object.h externs.h
mat4x4.o: mat4x4.c mat4x4.h vec4x1.h
player.o: player.c game.h vec4x1.h mat4x4.h camera.h system.h defines.h \
 object.h externs.h
vec4x1.o: vec4x1.c vec4x1.h
