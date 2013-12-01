# $Date: 2009-03-12 21:32:16 +0100 (Do, 12. Mär 2009) $
# $Revision: 97 $
#############################################################

.PHONY: __default_rule clean install nodebug \
	all_subdirs clean_subdirs install_subdirs nodebug_subdirs

__default_rule:	all

.SUFFIXES: .o

define run-cc
$(ECHO) "Compile $(notdir $<)"
$(CC) $(DEFINES) $(CFLAGS) -c $< -o $(OBJDIR)/$*.o
endef

#define run-cc-deps
#$(ECHO) "Compile $(notdir $<)"
#$(CC) $(DEFINES) $(CFLAGS) -c $< -o $(OBJDIR)/$*.o
#$(CC) -MM $(DEFINES) $(CFLAGS) -c $*.c -o $(OBJDIR)/$*.d
#$(FIXDEPS) $(OBJDIR)/$*.d
#endef

define build_subdir
$(MAKE) -s --directory=$(1) --file="makefile-new" TOPLEVEL="$(TOPLEVEL)" $(patsubst %_subdirs,%,$@);
endef

$(OBJDIR)/%.d :: %.c
	-@$(ECHO) "Update dependencies for $<"; \
	set -e; rm -f $@; \
	$(CC) -MM $(DEFINES) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


$(OBJDIR)/%.o: %.c
	@$(run-cc)
	
$(OBJDIR)/%.o : %.asm
	@$(ECHO) "Assemble $<"
	@$(AS) $(ASFLAGS) $< TO $(OBJDIR)/$*.obj
	@hunk2aout $(OBJDIR)/$*.obj >/dev/null
	@mv obj.0000.* $@

$(OBJDIR)/%.o : %.s
	@$(ECHO) "Assemble $<"
	@$(AS) $(ASFLAGS) $< TO $(OBJDIR)/$*.obj
	@hunk2aout $(OBJDIR)/$*.obj >/dev/null
	@mv obj.0000.* $@

nodebug:
	-@splat -s -o "d2(" "d1(" "#?.c"

all_subdirs clean_subdirs install_subdirs nodebug_subdirs:
	@$(foreach subdir,$(SUBDIRS),$(call build_subdir,$(subdir))) true

