##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

COMMON_DIR		= $(TOPLEVEL)/common/Plugin
BITMAPMCC_DIR		= $(TOPLEVEL)/common/BitMapMCC
BACKFILLMCC_DIR		= $(TOPLEVEL)/common/BackfillMCC
DATATYPESMCC_DIR	= $(TOPLEVEL)/common/DataTypesMCC

INCLUDES	+= 	-I$(COMMON_DIR) \
			-I$(BACKFILLMCC_DIR) \
			-I$(BITMAPMCC_DIR) \
			-I$(DATATYPESMCC_DIR)

SCALOS_LOCALE	= $(OBJDIR)/ScalosPattern_locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

vpath 	%.c	$(COMMON_DIR) $(BITMAPMCC_DIR) $(BACKFILLMCC_DIR) $(DATATYPESMCC_DIR)

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
