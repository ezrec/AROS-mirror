## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

SUB_DEMOS = book demos samples xdemos

# Selected subdirs
sel_sub_demos =
sel_sub_glut =
sel_sub_glu =

if HAVE_DEMO_SOURCE
sel_subdemos = $(SUB_DEMOS)
endif

if NEED_GLUT
sel_subglut = src-glut
endif

if USE_SI_GLU
sel_subglu = si-glu
else
sel_subglu = src-glu
endif

exec:
	@for subdir in $(sel_sub_demos); do \
	  echo "Making exec in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) exec); \
	done;

DIST_SUBDIRS = include src si-glu src-glu src-glut $(SUB_DEMOS) util
SUBDIRS = include src $(sel_subglu) $(sel_subglut) $(sel_subdemos) util

EXTRA_DIST = \
	3Dfx \
	BeOS \
	OpenStep \
	WIN32 \
	docs \
	ggi \
	images \
	m4 \
	macos \
	mtdemos \
	src-glut.beos \
	src-glut.dja \
	tests \
	vms \
	widgets-mesa \
	widgets-sgi \
	bootstrap \
	MESADOS.BAT \
	MESAFX.BAT \
	Make-config \
	Makefile.DJ \
	Makefile.X11 \
	Makefile.dja \
	Makefile.win \
	Mingw32.bat \
	Names.win \
	STARTUP.MK \
	common.dja \
	descrip.mms \
	fixam \
	mesaw32.BAT \
	mesawin32.mak \
	mms-config \
	win32-openstep.sh \
	xlib.opt

strip:
	@for subdir in src src-glu si-glu $(sel_sub_glut); do \
	  echo "Making strip in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) strip); \
	done;

dist-hook:
	rm -rf `find $(distdir) -name 'CVS' -type d`

include $(top_srcdir)/common_rules.make
