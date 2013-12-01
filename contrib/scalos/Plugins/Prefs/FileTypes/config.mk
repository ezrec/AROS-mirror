# $Date: 2012-08-10 13:47:28 +0200 (Fr, 10. Aug 2012) $
# $Revision: 915 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

FONTSAMPLE_DIR	= $(TOPLEVEL)/common/FontSampleMCC
ICONOBJMCC_DIR	= $(TOPLEVEL)/common/IconobjectMCC
DATATYPESMCC_DIR  = $(TOPLEVEL)/common/DataTypesMCC
COMMON_DIR	= $(TOPLEVEL)/common/Plugin

INCLUDES	+=	-I$(FONTSAMPLE_DIR) \
			-I$(ICONOBJMCC_DIR) \
			-I$(DATATYPESMCC_DIR) \
			-I$(COMMON_DIR)

SCALOS_LOCALE	= $(OBJDIR)/ScalosFileTypes_locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

vpath 	%.c	$(COMMON_DIR)

##############################################################################

include $(COMMON_DIR)/config.mk


##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

LFLAGS	+=	-nostartfiles \
		-lmempools
#		--verbose


OPTIONS	+=	-Wno-format-y2k


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS  +=      -nostartfiles \
#


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

DEFINES +=      -DMUI_OBSOLETE
LFLAGS  +=      -nostartfiles -lmui \
#


else

###############################################################################
# AmigaOS

LFLAGS	+=	-lscalos \
		-lpreferences \
		-liconobject \
		-lmempools \
		-ldebug \
		-lmui \
		-lstack \
		-lnix \
		-lnixmain \
		-lamiga21 \
		-lamiga \
		-lstubs \

endif
endif
endif
