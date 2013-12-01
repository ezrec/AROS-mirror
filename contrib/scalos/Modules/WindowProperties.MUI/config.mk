# $Date: 2011-07-16 18:28:30 +0200 (Sa, 16. Jul 2011) $
# $Revision: 784 $
#############################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

TOOLTYPE_DIR	=  ../IconProperties.MUI
ICONOBJMCC_DIR	= $(TOPLEVEL)/common/IconobjectMCC
BITMAPMCC_DIR	= $(TOPLEVEL)/common/BitMapMCC
BACKFILLMCC_DIR	= $(TOPLEVEL)/common/BackfillMCC

vpath	%.c	$(TOOLTYPE_DIR)	$(ICONOBJMCC_DIR) $(BITMAPMCC_DIR) $(BACKFILLMCC_DIR)

INCLUDES	+= 	-I$(TOOLTYPE_DIR) \
			-I$(BITMAPMCC_DIR) \
			-I$(BACKFILLMCC_DIR) \
			-I$(ICONOBJMCC_DIR)

SCALOS_LOCALE	= $(OBJDIR)/WindowProperties_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

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
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS	+=	-lmui -lutility


else

###############################################################################
# AmigaOS

LFLAGS	+=	#

endif
endif
endif
