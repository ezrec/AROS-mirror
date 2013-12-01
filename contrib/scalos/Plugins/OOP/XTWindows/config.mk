##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

COMMON_DIR	= $(TOPLEVEL)/common/Plugin

INCLUDES	+= -I$(COMMON_DIR)

vpath 	%.c	$(COMMON_DIR)

##############################################################################

include $(COMMON_DIR)/config.mk

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)


OPENDRAWERBYNAME_DIR  =  ../wb39_plugin

###############################################################################
# MorphOS

LFLAGS	+=	-nostartfiles \
#		-lmempools
#


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

LFLAGS  +=      -nostartfiles \
#


else

###############################################################################
# AmigaOS

LFLAGS	+=	-lscalos \
		-liconobject \
		-lmempools \
		-lstack \
		-lnix \
		-lnixmain \
		-lamiga21 \
		-lamiga \
		-lstubs \

endif
endif
endif
