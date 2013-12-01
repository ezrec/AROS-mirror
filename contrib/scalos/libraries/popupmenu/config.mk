##############################################################################
# $Date: 2009-02-17 21:22:13 +0100 (Di, 17. Feb 2009) $
# $Revision: 5 $
##############################################################################

ifndef TOPLEVEL
	TOPLEVEL=$(shell pwd)/../..
endif

###############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

###############################################################################
# Check compiler

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

SCALOS_INC	= scalos:Src/scalos

INCLUDES  +=    -I$(SCALOS_INC)/ppc-mos-include \
		-I$(SCALOS_INC)/include

LFLAGS	+=	-nostartfiles \
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

# Overwrite the DEFINES so that __NOLIBBASE__ is not defined
OS4LIBBASE = 	-D__USE_BASETYPE__

LFLAGS	+=	-nostartfiles \
#		--verbose

else

###############################################################################
# AmigaOS

LFLAGS	+=	-liconobject -lstack -lnix -lnixmain -lamiga -lstubs

endif
endif
