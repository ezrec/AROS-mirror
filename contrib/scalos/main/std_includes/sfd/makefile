# makefile for Scalos +jl+
# $Date$
# $Revision$

## generate fd/.fd, lvo/.asm, pragmas/.h, protos/.h from .sfd files

#-------------------------------------------------------------------------

SFD			= sfd
IDLTOOL			= idltool
FDTRANS			= fdtrans
C_INCDIR		= ///include
C_PPC_MOS_INCDIR 	= ///ppc-mos-include
ASMINCDIR		= /asm
XMLDIR			= /interfaces
FD_DIR			= fd:
EMPTY			=

#-------------------------------------------------------------------------

SFDS =          \
                scalosgfx \
	  	sqlite3 \
		openurl \
		scalos \
		iconobject \
		scalosmenuplugin \
		scalospreviewplugin \
		scalosfiletypeplugin \
		scalosplugin \
		preferences \
		pm \
		scalosprefsplugin \
		preferences \
		pm \
		mcpgfx \
		guigfx \
		dtlib \
		newicon \
		cybergraphics \

#-------------------------------------------------------------------------

.PHONY : all clean install

all: 	$(foreach sfd,$(SFDS),fd/$(sfd)_lib.fd)

#-------------------------------------------------------------------------

define genheaders
$(SFD) -t -h $<
-fd2pragma $(1)_lib.sfd SPECIAL 40 to $(C_INCDIR)/inline
-fd2pragma $(1)_lib.sfd SPECIAL 3 to $(C_INCDIR)/pragmas
-fd2pragma $(1)_lib.sfd SPECIAL 130 to $(C_PPC_MOS_INCDIR)/inline
-fd2pragma $(1)_lib.sfd SPECIAL 110 to $(FD_DIR)
-fd2pragma $(1)_lib.sfd SPECIAL 111 to $(C_INCDIR)/clib
-fd2pragma $(1)_lib.sfd SPECIAL 21 to $(ASMINCDIR)/LVO
$(FDTRANS) $(1)_lib.sfd --xml
copy $(1).xml $(XMLDIR)
$(IDLTOOL) $(1).xml --interface
copy include/interfaces/$(1).h $(C_INCDIR)/interfaces
$(IDLTOOL) $(1).xml --inline
copy include/inline4/$(1).h $(C_INCDIR)/inline4
delete $(1).xml
sed "s/END/; END/" LVO/$(1)_lib.asm >LVO/$(1)_lib2.asm
copy LVO/$(1)_lib2.asm 		LVO/$(1)_lib.asm
delete LVO/$(1)_lib2.asm
endef

#-------------------------------------------------------------------------

define copyheaders

endef

#-------------------------------------------------------------------------

fd/%_lib.fd pragmas/%_pragmas.h clib/%_protos.h : %_lib.sfd
	-$(call genheaders,$(subst _lib.sfd,$(EMPTY),$<))

#-------------------------------------------------------------------------

clean:
	-delete FD/#?.fd
	-delete LVO/#?.asm
	-delete PRAGMAS/#?.h
	-delete PROTOS/#?.h

#-------------------------------------------------------------------------

install:
	$(foreach sfd,$(SFDS),$(call copyheaders,$(sfd)))

#-------------------------------------------------------------------------
