CFLAGS        = /O2 /D__MSC__ /D__WIN32__ /DWIN32 /DFX  /I..\include \
                /I..\src \
                /I$(MSVC)\include
LDFLAGS	      =

OBJS          = fxevent.obj \
		font.obj \
		image.obj \
		shapes.obj 

PROGRAM       = ..\lib\fxTK.lib

all:		$(PROGRAM)


$(PROGRAM):     $(OBJS) $(LIBS)
		lib  /verbose /OUT:$(PROGRAM) $(OBJS)
