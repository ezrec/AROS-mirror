ifndef $(TOPLEVEL)
	TOPLEVEL=$(shell pwd)/..
endif

###############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

###############################################################################
# Check compiler

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

LFLAGS	+=	#


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS	+=	#


else

###############################################################################
# AmigaOS and AROS

LFLAGS	+=	#

endif
endif

