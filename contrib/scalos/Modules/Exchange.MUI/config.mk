# $Date: 2011-07-16 19:39:22 +0200 (Sa, 16. Jul 2011) $
# $Revision: 787 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

ICONOBJMCC_DIR	= $(TOPLEVEL)/common/IconobjectMCC

SCALOS_LOCALE	= $(OBJDIR)/Exchange_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \

LFLAGS	+=	#


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \

LFLAGS	+=	-lauto


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \

LFLAGS	+=	-lmui -lutility


else


###############################################################################
# AmigaOS

LFLAGS	+=	#

endif
endif
endif
