# makefile for all Scalos modules
# $Date$
# $Revision$

#############################################################

SUBDIRS =	Rename.Reaction \
		NewDrawer.Reaction \
		Delete.Gadtools \
		Empty_Trashcan.Gadtools \
		Empty_Trashcan.MUI \
		Execute_Command.Gadtools \
		NewDrawer.Gadtools \
		Rename.Gadtools \
		Exchange.MUI \
                Find.MUI \
                FormatDisk.Gadtools \
                Rename.MUI \
		Delete.MUI \
                NewDrawer.MUI \
		IconProperties.MUI \
		WindowProperties.MUI \
                Information.MUI \
                Execute_Command.MUI \
                Updater.MUI \

#############################################################

.PHONY: All install clean

#############################################################

define build_subdir
$(MAKE) -s --directory=$(1);
endef

define install_subdir
$(MAKE) -s install --directory=$(1);
endef

define clean_subdir
$(MAKE) -s clean --directory=$(1);
endef

#############################################################

All:
	@$(foreach subdir,$(SUBDIRS),$(call build_subdir,$(subdir)))

#############################################################

install:
	@$(foreach subdir,$(SUBDIRS),$(call install_subdir,$(subdir)))

#############################################################

clean:
	@$(foreach subdir,$(SUBDIRS),$(call clean_subdir,$(subdir)))

#############################################################
