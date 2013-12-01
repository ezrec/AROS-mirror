# $Date: 2011-07-16 19:39:22 +0200 (Sa, 16. Jul 2011) $
# $Revision: 787 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

TOOLTYPE_DIR	=  ../IconProperties.MUI
ICONOBJMCC_DIR	= $(TOPLEVEL)/common/IconobjectMCC
INT64_DIR	= $(TOPLEVEL)/common/Int64
FS_DIR	     	= $(TOPLEVEL)/common/Fs

vpath	%.c	$(TOOLTYPE_DIR) $(ICONOBJMCC_DIR) $(INT64_DIR) $(FS_DIR)

SCALOS_LOCALE	= $(OBJDIR)/Information_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

##############################################################################

INCLUDES	+= 	-I$(TOOLTYPE_DIR) \
			-I$(ICONOBJMCC_DIR) \
			-I$(INT64_DIR) \
			-I$(FS_DIR) \

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=

LFLAGS	+=	#


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

INCLUDES	+=

LFLAGS	+=	#


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

INCLUDES	+=

LFLAGS	+=	-lmui -lutility


else

###############################################################################
# AmigaOS

INCLUDES	+=

LFLAGS	+=	#

endif
endif
endif
