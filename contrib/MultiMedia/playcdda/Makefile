CC := i386-aros-gcc
RM := rm -f

CFLAGS := -O3 -Wall -Wno-pointer-sign -Iinclude -DMUI_OBSOLETE
LINK := -s
LIBS := -lmui

TARGET := PlayCDDA
VERSION := 1
OBJS := playcdda_gui.o scsicmd.o diskchange.o seekbar.o

../PlayCDDA/$(TARGET): $(OBJS)
	$(CC) $(LINK) -o $@ $^ $(LIBS)

playcdda_gui.o: $(TARGET)_rev.h

clean:
	$(RM) $(OBJS)

revision:
	bumprev $(VERSION) $(TARGET)
