# Makefile for core library for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl

.first
	define gl [-.include.gl]

.include [-]mms-config.

##### MACROS #####

VPATH = RCS

INCDIR = [-.include]
LIBDIR = [-.lib]
CFLAGS = /nowarn/include=($(INCDIR),[])/define=(FBIND=1)

CORE_SOURCES = accum.c alpha.c alphabuf.c api1.c api2.c apiext.c attrib.c \
bitmap.c blend.c clip.c colortab.c context.c copypix.c depth.c \
dlist.c drawpix.c enable.c eval.c feedback.c fog.c \
get.c hash.c image.c light.c lines.c logic.c masking.c matrix.c \
misc.c mmath.c mthreads.c pb.c pixel.c points.c pointers.c polygon.c \
quads.c rastpos.c readpix.c rect.c scissor.c shade.c span.c \
stencil.c teximage.c texobj.c texstate.c texture.c triangle.c \
varray.c winpos.c vb.c vbfill.c vbrender.c vbxform.c xform.c \
zoom.c

DRIVER_SOURCES = [.x]glxapi.c [.x]fakeglx.c [.x]realglx.c [.x]xfonts.c \
[.x]xmesa1.c [.x]xmesa2.c [.x]xmesa3.c [.x]xmesa4.c \
[.osmesa]osmesa.c \
[.svga]svgamesa.c \
[.fx]fxapi.c [.fx]fxdd.c [.fx]fxddtex.c [.fx]fxvsetup.c [.fx]fxsetup.c \
[.fx]fxpoints.c [.fx]fxlines.c [.fx]fxtris.c [.fx]fxquads.c \
[.fx]fxrender.c [.fx]fxtexman.c [.fx]fxddspan.c

ASM_SOURCES =

OBJECTS =\
accum.obj,alpha.obj,alphabuf.obj,api1.obj,api2.obj,apiext.obj,attrib.obj,\
bitmap.obj,blend.obj,clip.obj,colortab.obj,context.obj,copypix.obj,depth.obj,\
dlist.obj,drawpix.obj,enable.obj,eval.obj,feedback.obj,fog.obj,\
get.obj,hash.obj,image.obj,light.obj,lines.obj,logic.obj,masking.obj,matrix.obj,\
misc.obj,mmath.obj,mthreads.obj,pb.obj,pixel.obj,points.obj,pointers.obj,polygon.obj,\
quads.obj,rastpos.obj,readpix.obj,rect.obj,scissor.obj,shade.obj,span.obj,\
stencil.obj,teximage.obj,texobj.obj,texstate.obj,texture.obj,triangle.obj,\
varray.obj,winpos.obj,vb.obj,vbfill.obj,vbrender.obj,vbxform.obj,xform.obj,\
zoom.obj,\
[.x]glxapi.obj,[.x]fakeglx.obj,[.x]realglx.obj,[.x]xfonts.obj,\
[.x]xmesa1.obj,[.x]xmesa2.obj,[.x]xmesa3.obj,[.x]xmesa4.obj,\
[.osmesa]osmesa.obj,\
[.svga]svgamesa.obj,\
[.fx]fxapi.obj,[.fx]fxdd.obj,[.fx]fxddtex.obj,[.fx]fxvsetup.obj,[.fx]fxsetup.obj,\
[.fx]fxpoints.obj,[.fx]fxlines.obj,[.fx]fxtris.obj,[.fx]fxquads.obj,\
[.fx]fxrender.obj,[.fx]fxtexman.obj,[.fx]fxddspan.obj

##### RULES #####

##### TARGETS #####
# Make the library
$(LIBDIR)$(GL_LIB) : $(OBJECTS)
	$(MAKELIB) $(GL_LIB) $(OBJECTS)
	rename $(GL_LIB)* $(LIBDIR)

clean :
	purge
	delete *.obj;*

[.x]glxapi.obj : [.x]glxapi.c
	$(CC) $(CFLAGS) /obj=[.x]glxapi.obj [.x]glxapi.c
[.x]fakeglx.obj : [.x]fakeglx.c
	$(CC) $(CFLAGS) /obj=[.x]fakeglx.obj [.x]fakeglx.c
[.x]realglx.obj : [.x]realglx.c
	$(CC) $(CFLAGS) /obj=[.x]realglx.obj [.x]realglx.c
[.x]xfonts.obj : [.x]xfonts.c
	$(CC) $(CFLAGS) /obj=[.x]xfonts.obj [.x]xfonts.c
[.x]xmesa1.obj : [.x]xmesa1.c
	$(CC) $(CFLAGS) /obj=[.x]xmesa1.obj [.x]xmesa1.c
[.x]xmesa2.obj : [.x]xmesa2.c
	$(CC) $(CFLAGS) /obj=[.x]xmesa2.obj [.x]xmesa2.c
[.x]xmesa3.obj : [.x]xmesa3.c
	$(CC) $(CFLAGS) /obj=[.x]xmesa3.obj [.x]xmesa3.c
[.x]xmesa4.obj : [.x]xmesa4.c
	$(CC) $(CFLAGS) /obj=[.x]xmesa4.obj [.x]xmesa4.c
[.osmesa]osmesa.obj : [.osmesa]osmesa.c
	$(CC) $(CFLAGS) /obj=[.osmesa]osmesa.obj [.osmesa]osmesa.c
[.svga]svgamesa.obj : [.svga]svgamesa.c
	$(CC) $(CFLAGS) /obj=[.svga]svgamesa.obj [.svga]svgamesa.c
[.fx]fxapi.obj : [.fx]fxapi.c
	$(CC) $(CFLAGS) /obj=[.fx]fxapi.obj [.fx]fxapi.c
[.fx]fxdd.obj : [.fx]fxdd.c
	$(CC) $(CFLAGS) /obj=[.fx]fxdd.obj [.fx]fxdd.c
[.fx]fxddtex.obj : [.fx]fxddtex.c
	$(CC) $(CFLAGS) /obj=[.fx]fxddtex.obj [.fx]fxddtex.c
[.fx]fxvsetup.obj : [.fx]fxvsetup.c
	$(CC) $(CFLAGS) /obj=[.fx]fxvsetup.obj [.fx]fxvsetup.c
[.fx]fxsetup.obj : [.fx]fxsetup.c
	$(CC) $(CFLAGS) /obj=[.fx]fxsetup.obj [.fx]fxsetup.c
[.fx]fxpoints.obj : [.fx]fxpoints.c
	$(CC) $(CFLAGS) /obj=[.fx]fxpoints.obj [.fx]fxpoints.c
[.fx]fxlines.obj : [.fx]fxlines.c
	$(CC) $(CFLAGS) /obj=[.fx]fxlines.obj [.fx]fxlines.c
[.fx]fxtris.obj : [.fx]fxtris.c
	$(CC) $(CFLAGS) /obj=[.fx]fxtris.obj [.fx]fxtris.c
[.fx]fxquads.obj : [.fx]fxquads.c
	$(CC) $(CFLAGS) /obj=[.fx]fxquads.obj [.fx]fxquads.c
[.fx]fxrender.obj : [.fx]fxrender.c
	$(CC) $(CFLAGS) /obj=[.fx]fxrender.obj [.fx]fxrender.c
[.fx]fxtexman.obj : [.fx]fxtexman.c
	$(CC) $(CFLAGS) /obj=[.fx]fxtexman.obj [.fx]fxtexman.c
[.fx]fxddspan.obj : [.fx]fxddspan.c
	$(CC) $(CFLAGS) /obj=[.fx]fxddspan.obj [.fx]fxddspan.c

.include mms_depend.
