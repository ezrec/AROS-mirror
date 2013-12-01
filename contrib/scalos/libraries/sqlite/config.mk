# $Date: 2012-08-10 13:47:28 +0200 (Fr, 10. Aug 2012) $
# $Revision: 915 $
##############################################################################

ifndef TOPLEVEL
	TOPLEVEL=$(shell pwd)/../..
endif

###############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

###############################################################################

SRCDIR	=	./src

DEFINES +=      -DNO_TCL=1 \
		-DHAVE_USLEEP=0 \
		-DSQLITE_THREADSAFE=1 \
		-DSQLITE_MUTEX_NOOP=1 \
		-DSQLITE_EXTRA \
		-DSQLITE_OS_OTHER=1 \
		-DSQLITE_OMIT_UTF16=1 \
		-DSQLITE_ENABLE_MEMORY_MANAGEMENT \
		-DSQLITE_ENABLE_COLUMN_METADATA \
		-DSQLITE_OMIT_AUTHORIZATION=1 \
		-DTEMP_FILE_PREFIX="t:etilqs_" \
#		-DSQLITE_DEBUG


INCLUDES +=	-I./src

###############################################################################
# Check compiler

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

LFLAGS	+=	-nostartfiles \
		-lmempools \
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS	+=	-nostartfiles

else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS	+=	-nostartfiles -larossupport

else

###############################################################################
# AmigaOS

LFLAGS  +=  	-specs=gg:etc/specs -nostdlib -nostartfiles \
		-lm \
		-lmempools \
		-ldebug \
		-lgcc \
		-lnix \
		-lnixmain \
		-lamiga21 \
		-lamiga \
		-lstubs

endif
endif
endif
