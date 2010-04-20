CC     := gcc
CFLAGS := -O3
LIBS   := -larossupport -lrom

TARGET := disassembler.library
INCLUDES := -Ibfd -Iopcodes/include -isystem Include

FILES := library.c disassemble.c findstartposition.c support.c
OBJS  := library.o disassemble.o findstartposition.o support.o

all: $(TARGET)

$(TARGET): $(OBJS) includes libopcodes
	$(CC) $(LDFLAGS) $^ -o $@ -L opcodes -lopcodes $(LIBS) -nostartfiles

includes:
	make -C Include

libopcodes:
	make -C opcodes

support.o: support.c support.h bfd/bfd.h opcodes/include/dis-asm.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

disassemble.o: disassemble.c support.h bfd/bfd.h opcodes/include/dis-asm.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

findstartposition.o: findstartposition.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

library.o: library.c library.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

clean:
	rm -f *.o $(TARGET)
	make clean -C opcodes
	make clean -C include

distclean:
	rm -f *.o $(TARGET)
	make distclean -C opcodes
	make clean -C include
