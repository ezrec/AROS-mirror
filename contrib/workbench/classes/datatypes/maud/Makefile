CC = vc +aosppc
RM = delete
CP = copy
MAKEDT = RX REXX:MakeDT.rexx
CFLAGS = -O3 -Iinclude
LIBS =

TARGET  = maud.datatype
DT_DESC = MAUD
VERSION = 52

OBJS = init.o class.o decoders.o maud_pcm.o bitpack.o bitpack_msb.o \
	maud_alaw.o

all: $(TARGET) $(DT_DESC)

$(TARGET): $(OBJS)
	$(CC) -nostdlib -o $@ $^ $(LIBS)
	protect $@ +e

init.o: $(TARGET)_rev.h

$(DT_DESC): $(DT_DESC).dt
	$(MAKEDT) $(DT_DESC).dt $(DT_DESC)

install: $(TARGET) $(DT_DESC)
	$(CP) $(TARGET) SYS:Classes/DataTypes/
	$(CP) $(DT_DESC) DEVS:DataTypes/

clean:
	$(RM) $(OBJS)

revision:
	bumprev $(VERSION) $(TARGET)
