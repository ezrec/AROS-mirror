# Makefile for book programs


# $Id$

# $Log$
# Revision 1.1  2003/08/09 00:22:56  chodorowski
# Initial revision
#
# Revision 1.4  1995/08/01  20:59:05  brianp
# cleaned up, use $(xx_LIB) variable
#
# Revision 1.3  1995/03/13  16:04:47  brianp
# added xfont to PROGS
#
# Revision 1.2  1995/03/04  19:43:29  brianp
# updated for Make-config
#
# Revision 1.1  1995/03/03  14:38:09  brianp
# Initial revision
#


##### MACROS #####

INCDIR = ../include

GL_LIBS = -L../lib -lMesaaux -lMesatk -lMesaGLU -lMesaGL -lm $(XLIBS)

LIB_DEP = ../lib/$(GL_LIB) ../lib/$(GLU_LIB) ../lib/$(TK_LIB) ../lib/$(AUX_LIB)

PROGS = robot accanti accnot accpersp accum aim alpha alpha3D \
	anti antiindex antipindex antipoint antipoly \
	bezcurve bezmesh bezsurf checker checker2 \
	chess clip colormat cone cube curve \
	depthcue disk dof dofnot double drawf \
	feedback fog fogindex font light linelist \
	lines list list2 maplight material mipmap \
	model movelight nurbs pickdepth pickline \
	picksquare plane planet planetup polys \
	robot sccolorlight scene scenebamb sceneflat \
	select simple smooth sphere stencil stroke \
	surface tea teaambient teapots texgen texturesurf xfont

ifdef AROS
    PROGS = robot accanti accnot accpersp accum aim alpha alpha3D \
		anti antiindex antipindex antipoint antipoly \
		bezcurve bezmesh bezsurf checker checker2 \
		chess clip colormat cone cube curve \
		depthcue disk dof dofnot double drawf \
        	feedback fog fogindex font light linelist \
        	lines list list2 maplight material mipmap \
        	model movelight nurbs pickdepth pickline \
        	picksquare plane planet planetup polys \
        	robot sccolorlight scene scenebamb sceneflat \
        	select simple smooth sphere stencil stroke \
        	surface tea teaambient teapots texgen texturesurf
endif

##### RULES #####

.SUFFIXES:
.SUFFIXES: .c

.c: $(LIB_DEP)
	$(CC) -I$(INCDIR) $(CFLAGS) $< $(GL_LIBS) -o $@



##### TARGETS ######

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

realclean:
	-rm $(PROGS)
	-rm *.o *~

targets: $(PROGS)

include ../Make-config

