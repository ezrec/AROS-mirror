# Makefile for tk toolkit for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl


.first
	define gl [-.include.gl]

.include [-]mms-config.

##### MACROS #####

VPATH = RCS

INCDIR = [-.include]
LIBDIR = [-.lib]
CFLAGS = /include=$(INCDIR)/define=(FBIND=1)

OBJECTS = cursor.obj,event.obj,font.obj,getset.obj,image.obj,shapes.obj,\
	window.obj



##### RULES #####


##### TARGETS #####

# Make the library
$(LIBDIR)$(TK_LIB) : $(OBJECTS)
	$(MAKELIB) $(TK_LIB) $(OBJECTS)
	rename $(TK_LIB)* $(LIBDIR)

clean :
	delete *.obj;*
	purge

