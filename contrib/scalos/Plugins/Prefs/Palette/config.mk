##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

DATATYPESMCC_DIR  = $(TOPLEVEL)/common/DataTypesMCC
COMMON_DIR	= $(TOPLEVEL)/common/Plugin

INCLUDES	+= -I$(DATATYPESMCC_DIR) -I$(COMMON_DIR)

SCALOS_LOCALE	= $(OBJDIR)/ScalosPalette_locale.h

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
