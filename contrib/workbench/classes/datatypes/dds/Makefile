MAKE   = make
CC     = vc +aosppc
CP	   = copy
RM     = delete
MAKEDT = RX REXX:MakeDT.rexx

# Change these as required
OPTIMIZE = -O3 -size
DEBUG = 
CFLAGS = $(OPTIMIZE) $(DEBUG) -I. -Iinclude

# Flags passed to gcc during linking
LINK = $(OPTIMIZE) $(DEBUG)

# Additional linker libraries
LIBS = 

# Name of the "thing" to build
TARGET = dds.datatype
DT_DESC = DDS
VERSION = 52

# Source code files used in this project
# Add any additional files to this line

OBJS = init.o dds_class.o rgb_read.o dxt_read.o

# -------------------------------------------------------------
# Nothing should need changing below this line

# Rules for building
all: $(TARGET) $(DT_DESC)

$(TARGET): $(OBJS)
	$(CC) $(LINK) -final -nostdlib -o $(TARGET) $(OBJS) $(LIBS)

init.o: $(TARGET)_rev.h

$(DT_DESC): $(DT_DESC).dt
	$(MAKEDT) $(DT_DESC).dt $(DT_DESC)

.PHONY: install
install: $(TARGET) $(DT_DESC)
	$(CP) $(TARGET) SYS:Classes/DataTypes/
	$(CP) $(DT_DESC) DEVS:DataTypes/

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: distclean
distclean:
	$(RM) $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)

