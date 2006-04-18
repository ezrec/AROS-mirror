## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -I$(top_srcdir)/include $(GLUT_CFLAGS)

check_PROGRAMS = bounce \
	clearspd \
	cubemap \
	drawpix \
	fire \
	gamma \
	gears \
	geartrain \
	glinfo \
	gloss \
	gltestperf \
	glutfx \
	isosurf \
	ipers \
	lodbias \
	morph3d \
	multiarb \
	occlude \
	osdemo \
	paltex \
	pixeltex \
	pointblast \
	ray \
	readpix \
	reflect \
	renormal \
	spectex \
	stex3d \
	teapot \
	terrain \
	tessdemo \
	texcyl \
	texdown \
	texenv \
	texobj \
	trispd \
	tunnel \
	tunnel2 \
	winpos



CLEANFILES = test.ppm

LDADD = $(GLUT_LIBS) $(top_builddir)/src-glu/libGLU.la \
	$(top_builddir)/src/libGL.la -lm
LDFLAGS = -no-install

EXTRA_DIST = isosurf.dat

# use double buffering if possible
BUFFER = -db

# execute all programs
exec: $(check_PROGRAMS)
	@for prog in $(check_PROGRAMS);	\
	do				\
	 echo -n "Running $$prog ..." ;	\
	 case "$$prog" in		\
	 osdemo) ./$$prog test.ppm ;; 	\
	 stex3d) ./$$prog $(BUFFER) ;; 	\
	 *) ./$$prog ;;			\
	 esac ;				\
	 echo ;				\
	done
