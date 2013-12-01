##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################
# Check gcc

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
# AmigaOS

LFLAGS	+=	-lscalos -lpreferences -lstack -lnix -lnixmain -lamiga2 -lamiga -lstubs

endif
endif

