MAKE   = make
CC     = vc
CP	   = copy
RM     = delete
MAKEDT = RX REXX:MakeDT.rexx

# Change these as required
OPTIMIZE = -O4 -speed
DEBUG = 
CFLAGS = $(OPTIMIZE) $(DEBUG) -I. -Iinclude

# Flags passed to gcc during linking
LINK = $(OPTIMIZE) $(DEBUG)

# Additional linker libraries
LIBS = -lvc

# Name of the "thing" to build
TARGET = jpeg2000.datatype
VERSION = 0

# Source code files used in this project
# Add any additional files to this line

OBJS = init.o jp2_class.o

# -------------------------------------------------------------
# Nothing should need changing below this line

# Rules for building
all: $(TARGET) JP2 J2K

$(TARGET): $(OBJS)
	$(CC) $(LINK) -nostdlib -o $@ $^ $(LIBS)

init.o: $(TARGET)_rev.h

JP2: JP2.dt
	$(MAKEDT) $< $@

J2K: J2K.dt
	$(MAKEDT) $< $@

.PHONY: install
install: all
	$(CP) $(TARGET) SYS:Classes/DataTypes/
	$(CP) JP2 J2K DEVS:DataTypes/

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: distclean
distclean:
	$(RM) $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)

