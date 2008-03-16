# TheBar.mcc TheBarVirt.mcc "build" makefile
#
# This script is used by "makefile" and can't be called directly

# what are we going to build
ifeq ($(T),mcc)
  NAME     = $(TNAME)
  ODIR     = $(OBJDIR)/mcc
  LDFLAGS += -Wl,-Map,$@.map
else
ifeq ($(T),test)
  NAME     = $(TTNAME)
  ODIR     = $(OBJDIR)/mcc
  OBJS     = $(TESTOBJS)
  LDFLAGS := $(filter-out -nostartfiles,$(LDFLAGS))
  CFLAGS  := $(filter-out -nostartfiles,$(CFLAGS))
else
  NAME     = $(TVNAME)
  ODIR     = $(OBJDIR)/mccv
  DMORE    = -DVIRTUAL
  LDFLAGS += -Wl,-Map,$@.map
endif
endif

# set vars
TARGET  = $(BINDIR)/$(NAME)
VPATH   = $(ODIR)
CFLAGS += $(DMORE)
FOBJS   = $(addprefix $(ODIR)/,$(OBJS))

# main target
.PHONY: all
all: $(TARGET)

# build
$(TARGET): $(OBJS)
	@echo "  LD $(notdir $@)"
	@$(CC) $(LDFLAGS) -o $@.db $(FOBJS) $(M68KSTUBS) $(LDLIBS) -Wl,-Map,$@.map
	@$(STRIP) --remove-section=.comment -o $@ $@.db

# compile
$(OBJS):
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $(ODIR)/$@

# include dependencies file
include $(DEPFILE)

