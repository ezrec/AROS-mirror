!include <win32.mak>

CFLAGS        = $(cvarsdll) /Ox /G5 /Gz \
                /D__MSC__ /DFX /DFX_SILENT /D__WIN32__ /DWIN32 /I..\include /I.

OBJS            = accum.obj alpha.obj alphabuf.obj api1.obj api2.obj apiext.obj attrib.obj \
                bitmap.obj blend.obj clip.obj colortab.obj context.obj copypix.obj depth.obj \
                dlist.obj drawpix.obj enable.obj eval.obj feedback.obj fog.obj \
                get.obj hash.obj image.obj light.obj lines.obj logic.obj masking.obj matrix.obj \
                misc.obj mmath.obj pb.obj pixel.obj points.obj pointers.obj polygon.obj \
                quads.obj rastpos.obj readpix.obj rect.obj scissor.obj shade.obj span.obj \
                stencil.obj teximage.obj texobj.obj texstate.obj texture.obj triangle.obj \
                varray.obj winpos.obj vb.obj vbfill.obj vbrender.obj vbxform.obj xform.obj \
                zoom.obj


FXOBJS          = FX\fxwgl.obj FX\fxapi.obj FX\fxdd.obj FX\fxddtex.obj \
                FX\fxvsetup.obj FX\fxsetup.obj \
                FX\fxtexman.obj FX\fxrender.obj FX\fxpoints.obj \
                FX\fxlines.obj FX\fxtris.obj FX\fxquads.obj FX\fxddspan.obj

OSOBJS			= OSmesa\osmesa.obj

PROGRAM       = ..\lib\OpenGL32.DLL

all:		$(PROGRAM)

FX\fxapi.obj: FX\fxapi.c

FX\fxdd.obj: FX\fxdd.c

FX\fxddtex.obj: FX\fxddtex.c

FX\fxvsetup.obj: FX\fxvsetup.c

FX\fxsetup.obj: FX\fxsetup.c

FX\fxtexman.obj: FX\fxtexman.c

FX\fxrender.obj: FX\fxrender.c

FX\fxpoints.obj: FX\fxpoints.c

FX\fxlines.obj: FX\fxlines.c

FX\fxtris.obj: FX\fxtris.c

FX\fxquads.obj: FX\fxquads.c

FX\fxwgl.obj: FX\fxwgl.c

FX\fxddspan.obj: FX\fxddspan.c

OSmesa\osmesa.obj: OSmesa\osmesa.c

$(PROGRAM):     $(OBJS) $(FXOBJS) $(OSOBJS)
                $(link) $(dlllflags) /out:$(PROGRAM) \
                 /def:FX\fxOpenGL.def $(OBJS) $(FXOBJS) $(OSOBJS) $(guilibsdll) \
                 glide2x.lib >link.log

.c.obj:
        $(CC) $(CFLAGS) $< /c /Fo$@
