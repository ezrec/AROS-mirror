
MAKE   = make
CC     = vc +aosppc
CP	   = copy
RM     = delete
MAKEDT = RX REXX:MakeDT.rexx

# enable/disable g72x support
G72X = 1

ifeq "$(G72X)" "1"
G72XOPT = -DG72X_SUPPORT=1
G72XSRC = au_g72x.c g72x/g72x.c g72x/g721.c g72x/g723_24.c g72x/g723_40.c \
	bitpack.c bitpack_msb.c
else
G72XOPT =
G72XSRC =
endif

# Change these as required
OPTIMIZE = -O4 -size
DEBUG =
CFLAGS = $(OPTIMIZE) $(DEBUG) -I. -Iinclude $(G72XOPT)

# Flags passed to gcc during linking
LINK =

# Name of the "thing" to build
TARGET  = au.datatype
DT_DESC = AU
VERSION = 50

# Additional linker libraries
LIBS = -lm

# Source code files used in this project
# Add any additional files to this line

SRCS = au_lib.c au_class.c decoders.c au_pcm.c au_alaw.c au_float.c \
	$(G72XSRC)

# -------------------------------------------------------------
# Nothing should need changing below this line

OBJS = $(SRCS:.c=.o)

# Rules for building
all: $(TARGET) $(DT_DESC)

$(TARGET): $(OBJS)
	$(CC) $(LINK) -nostdlib -o $(TARGET) $(OBJS) $(LIBS)

$(DT_DESC): $(DT_DESC).dt
	$(MAKEDT) $(DT_DESC).dt $(DT_DESC)

.PHONY: install
install: $(TARGET) $(DT_DESC)
	$(CP) $(TARGET) SYS:Classes/DataTypes/
	$(CP) $(DT_DESC) DEVS:DataTypes/

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)

