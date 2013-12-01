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
# AmigaOS

LFLAGS	+=	-liconobject -lpm -lguigfx -lpreferences -lcybergraphics \
		-lscalos -lmcpgfx -lgcc -lnix -lnixmain \
		-lamiga21 -lamiga -lstubs

endif
endif

