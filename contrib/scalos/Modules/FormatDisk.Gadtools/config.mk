# $Date: 2010-05-14 21:40:32 +0200 (Fr, 14. Mai 2010) $
# $Revision: 612 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

SCALOS_LOCALE	= $(OBJDIR)/FormatDisk_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)
DEFINES += -DSCALOSLOCALE=$(SCALOS_LOCALE)

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

DEFINES	+= -DINTUITION_PRE_V36_NAMES

else

###############################################################################
# AmigaOS

INCLUDES	+= 	#

LFLAGS	+=	#

endif
endif

