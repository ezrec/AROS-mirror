MAKE   = make
CC     = vc
CP	   = copy
RM     = delete
MAKEDT = RX REXX:MakeDT.rexx

# Change these as required
OPTIMIZE = -O4 -size
DEBUG = 
CFLAGS = $(OPTIMIZE) $(DEBUG) -I. -Iinclude

# Flags passed to gcc during linking
LINK = $(OPTIMIZE) $(DEBUG)

# Additional linker libraries
LIBS = -lvc

# Name of the "thing" to build
TARGET = icns.datatype
DT_DESC = ICNS
VERSION = 52

# Source code files used in this project
# Add any additional files to this line

OBJS = init.o icns_class.o icns_elements.o colormaps.o rle24_decode.o jp2_decode.o

# -------------------------------------------------------------
# Nothing should need changing below this line

# Rules for building
all: $(TARGET) $(DT_DESC)

$(TARGET): $(OBJS)
	$(CC) $(LINK) -nostdlib -o $(TARGET) $(OBJS) $(LIBS)

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

