#  $Id$
#
#  Makefile.in for the examples
#

#
# Autoconfig stuff
########################################



top_srcdir	= ..
srcdir		= .
DISTDIR		= /tmp/i386-linux-aros-ahi

DEVDIR		= ${DISTDIR}/Developer
USRDIR		= ${DISTDIR}/User

DEVEXDIR	= ${DEVDIR}/Examples
USREXDIR	= ${USRDIR}/Examples

build_os	= linux-gnu

INSTALL		= /bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644




#
# Files
########################################

TARGETS		= 


#
# Targets
########################################

.PHONY:		all clean distclean maintainer-clean bindist

all:		$(TARGETS)

clean:
	$(RM) $(TARGETS) *.bak *~ \#*\# core

distclean:	clean
	$(RM) Makefile

maintainer-clean: distclean

install:

bindist:	$(TARGETS)
	$(INSTALL) --directory $(DEVEXDIR) #$(USREXDIR)
	for d in Device Extras Low-level; do \
	  cd $(srcdir) && find $${d} -type f -and -not -path '*CVS*' \
	    -exec $(INSTALL_DATA) -D {} $(DEVEXDIR)/{} \; ; \
	done
#	for d in Extras; do \
#	  find $${d} -type f -and -not -path '*CVS*' \
#	    -exec $(INSTALL_DATA) -D {} $(USREXDIR)/{} \; ; \
#	done


#
# Auto-remake autoconf stuff
########################################

Makefile: Makefile.in ../config.status
	(cd .. && ./config.status)


#
# Rules
########################################

