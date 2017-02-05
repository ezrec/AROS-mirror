CC     = gcc -mcrt=newlib
CP	   = copy
RM     = delete
MAKEDT = RX REXX:MakeDT.rexx

# Change these as required
OPTIMIZE = -O2
DEBUG = 
CFLAGS = $(OPTIMIZE) $(DEBUG) -Iinclude -Iamrnb

# Flags passed to gcc during linking
LINK = $(OPTIMIZE) $(DEBUG)

# Additional linker libraries
LIBS = 

# Name of the "thing" to build
TARGET  = amr.datatype
DT_DESC = AMR
VERSION = 0

# Source code files used in this project
# Add any additional files to this line

SRCS = init.c class.c amrnb/interf_dec.o amrnb/sp_dec.o

# -------------------------------------------------------------
# Nothing should need changing below this line

OBJS = $(SRCS:.c=.o)

# Rules for building
all: $(TARGET) $(DT_DESC)

$(TARGET): $(OBJS)
	$(CC) $(LINK) -nostartfiles -o $(TARGET) $(OBJS) $(LIBS)

init.o: $(TARGET)_rev.h

$(DT_DESC): $(DT_DESC).dt
	$(MAKEDT) $(DT_DESC).dt $(DT_DESC)

.PHONY: install
install: $(TARGET) $(DT_DESC)
	$(CP) $(TARGET) SYS:Classes/DataTypes/
	$(CP) $(DT_DESC) DEVS:DataTypes/

.PHONY: distclean
clean:
	$(RM) $(TARGET)

.PHONY: distclean
distclean:
	$(RM) $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)
