# Makefile for book programs


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

# execute all programs
exec: $(PROGS)
	@for prog in $(PROGS) ;			\
	do					\
		echo -n "Running $$prog ..." ;	\
		$$prog ;			\
		echo ;				\
	done


include ../Make-config
