# Makefile for sample programs

# $Id$

# $Log$
# Revision 1.1  2003/08/09 00:23:07  chodorowski
# Initial revision
#
# Revision 1.5  1995/08/01  20:59:49  brianp
# use $(TK_LIB) and $(AUX_LIB), added nurb demo
#
# Revision 1.4  1995/05/01  15:25:51  brianp
# clean up and reorganize
#
# Revision 1.3  1995/03/30  22:30:56  brianp
# added new demos to PROGS
#
# Revision 1.2  1995/03/04  19:43:47  brianp
# updated for Make-config
#
# Revision 1.1  1995/03/03  14:35:18  brianp
# Initial revision
#


##### MACROS #####

INCDIR = ../include

GL_LIBS = -L../lib -lMesatk -lMesaGLU -lMesaGL -lm $(XLIBS)

LIB_DEP = ../lib/$(GL_LIB) ../lib/$(GLU_LIB) ../lib/$(TK_LIB) ../lib/$(AUX_LIB)

ifdef AMIGA
PROGS = accum bitmap1 bitmap2 blendeq blendxor copy cursor depth \
	eval fog font line logo nurb \
	overlay point prim quad select shape \
	sphere star stencil stretch texture \
	tri wave
else
PROGS = accum bitmap1 bitmap2 blendeq blendxor copy cursor depth \
	eval fog font line logo nurb oglinfo olympic \
	overlay point prim quad select shape \
	speed sphere star stencil stretch texture \
	tri wave
endif


##### RULES #####

.SUFFIXES:
.SUFFIXES: .c

.c: $(LIB_DEP)
	$(CC) -I$(INCDIR) $(CFLAGS) $< $(GL_LIBS) -o $@



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

realclean:
	-rm $(PROGS)
	-rm *.o *~

targets: $(PROGS)

include ../Make-config


