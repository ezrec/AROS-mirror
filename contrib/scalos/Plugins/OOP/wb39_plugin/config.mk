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

INCLUDES	+= -I. -Ivolumegauge -Iwbrexx

vpath	%.c	volumegauge

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

LFLAGS	+=      -lmempools
#		--verbose

LFLAGS2 =	$(LFLAGS) \
		-nostartfiles

else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS2  =      $(LFLAGS) \
		-nostartfiles \
#

#INCLUDES	+= -I/Werk/Programming/WBStart/dev/OS4/include -I/Werk/Programming/WBStart/dev/c/include

else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS2  =      $(LFLAGS) \
		-nostartfiles \
#

else

###############################################################################
# AmigaOS

LFLAGS	+=	-lscalos \
		-lpreferences \
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
