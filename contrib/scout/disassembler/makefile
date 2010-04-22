CC     := gcc
CFLAGS := -O3
LIBS   := -larossupport -lrom

TARGET := disassembler.library
INCLUDES := -Ibfd -Iopcodes/include -isystem Include

FILES := library.c disassemble.c findstartposition.c libbfd.c libc.c object.c support.c
OBJS  := $(FILES:%.c=%.o)

all: $(TARGET)

$(TARGET): $(OBJS) includes libopcodes
	$(CC) $(LDFLAGS) $(OBJS) -o $@ -L opcodes -lopcodes $(LIBS) -nostartfiles

includes:
	make -C Include

libopcodes:
	make -C opcodes

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

disassemble.o: library.h object.h bfd/bfd.h

libbfd.o: libbfd.c bfd/bfd.h

libc.o: libc.h

library.o: library.h bfd/bfd.h

object.o: library.h object.h libc.h object_intern.h support.h bfd/bfd.h opcodes/include/dis-asm.h

support.o: library.h support.h bfd/bfd.h opcodes/include/dis-asm.h

clean:
	rm -f *.o $(TARGET)
	make clean -C opcodes
	make clean -C include

distclean:
	rm -f *.o $(TARGET)
	make distclean -C opcodes
	make clean -C include
