# Makefile for sample programs



##### MACROS #####

INCDIR = ../include

GL_LIBS = -L../lib -lMesatk -lMesaGLU -lMesaGL -lm $(XLIBS)

LIB_DEP = ../lib/$(GL_LIB) ../lib/$(GLU_LIB) ../lib/$(TK_LIB) ../lib/$(AUX_LIB)

PROGS = accum bitmap1 bitmap2 blendeq blendxor copy cursor depth \
	eval fog font line logo nurb oglinfo olympic \
	overlay point prim quad select shape \
	speed sphere star stencil stretch texture \
	tri wave



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

# execute all programs
exec: $(PROGS)
	@for prog in $(PROGS) ;			\
	do					\
		echo -n "Running $$prog ..." ;	\
		$$prog ;			\
		echo ;				\
	done


include ../Make-config
