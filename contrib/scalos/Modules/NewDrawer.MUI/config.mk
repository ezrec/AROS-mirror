# $Date: 2011-07-12 03:41:40 +0200 (Di, 12. Jul 2011) $
# $Revision: 777 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

SCALOS_LOCALE	= $(OBJDIR)/NewDrawer_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+= 	#

LFLAGS	+=	#


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

INCLUDES	+= 	#

LFLAGS	+=	#


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

INCLUDES	+= 	#

LFLAGS	+=	-lmui


else

###############################################################################
# AmigaOS

INCLUDES	+= 	#

LFLAGS	+=	#

endif
endif
endif
