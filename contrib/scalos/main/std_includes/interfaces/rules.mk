# $Date: 2007-05-29 19:45:39 +0200 (Di, 29 Mai 2007) $
# $Revision: 2495 $
##############################################################################

define build-os4-lib-files
FILES	+=	$(1).xml ../../../include/interfaces/$(1).h \
		../../../include/inline4/$(1).h \
		$(2)/$(1)-aos4-68kstubs.c

$(1).xml: ../sfd/$(1)_lib.sfd
	@$(ECHO) Generating xml file for $(1)
	@FDTrans -o $(1) /sfd/$(1)_lib.sfd

.files/include/interfaces/$(1).h .files/include/inline4/$(1).h: $(1).xml
	@$(ECHO) Generating OS4 includes for $(1)
	@cd .files; idltool -i /$(1).xml; idltool -n /$(1).xml

.files/$(1)_68k.c: ../sfd/$(1)_lib.sfd
	@$(ECHO) Generating m68k stubs for $(1)
	@cd .files; FDTrans -o $(1) -c //sfd/$(1)_lib.sfd

../../../include/interfaces/$(1).h: .files/include/interfaces/$(1).h
	@$(CP) $$< $$@

../../../include/inline4/$(1).h: .files/include/inline4/$(1).h
	@$(CP) $$< $$@

$(2)/$(1)-aos4-68kstubs.c: .files/$(1)_68k.c
	@$(CP) $$< $$@
endef

define build-os4-lib-files-68kcall
FILES	+=	$(1).xml ../../../include/interfaces/$(1).h \
		../../../include/inline4/$(1).h \
		$(2)/$(1)-aos4-68kstubs.c

$(1).xml: ../sfd/$(1)_lib.sfd
	@$(ECHO) Generating xml file for $(1)
	@FDTrans -o $(1) /sfd/$(1)_lib.sfd

.files/include/interfaces/$(1).h .files/$(1).h: $(1).xml
	@$(ECHO) Generating OS4 includes for $(1)
	@cd .files; idltool -i /$(1).xml; fdtrans -4 //sfd/$(1)_lib.sfd

.files/$(1)_68k.c: ../sfd/$(1)_lib.sfd
	@$(ECHO) Generating m68k stubs for $(1)
	@cd .files; FDTrans -o $(1) -c //sfd/$(1)_lib.sfd

../../../include/interfaces/$(1).h: .files/include/interfaces/$(1).h
	@$(CP) $$< $$@

../../../include/inline4/$(1).h: .files/$(1).h
	@$(CP) $$< $$@

$(2)/$(1)-aos4-68kstubs.c: .files/$(1)_68k.c
	@$(CP) $$< $$@
endef

##############################################################################

