CC=../../bin/linux-i386/tools/i386-linux-aros-gcc

LWIP_DIR    = ./lwip
LWIP_INCS   = -I$(LWIP_DIR)/src/include
LWIP_INCS  += -I$(LWIP_DIR)/proj/aros
LWIP_INCS  += -I$(LWIP_DIR)/src/arch/aros/include
LWIP_INCS  += -I$(LWIP_DIR)/src/include/ipv4
LWIP_LIBDIR = -L$(LWIP_DIR)/proj/aros

PKTDRV_DIR    = ./pktdrv
PKTDRV_INCS   = -I$(PKTDRV_DIR)
PKTDRV_LIBDIR = -L$(PKTDRV_DIR)

CFLAGS=-Wall -g -O2 $(LWIP_INCS) $(PKTDRV_INCS) -Iapps # -DLWIP_DEBUG
LIBS= -llwip4 $(LWIP_LIBDIR) -lpktdrv $(PKTDRV_LIBDIR)

OBJS= test.o perf.o sys_arch.o pktif.o \
      apps/httpd.o apps/fs.o apps/fsdata.o

main: $(OBJS) $(LWIP_LIBDIR)/liblwip4.a $(PKTDRV_LIBDIR)/libpktdrv.a
	$(CC) $(CFLAGS) -o main.exe $(OBJS) $(LIBS)

$(LWIP_LIBDIR)/liblwip4.a:
	$(MAKE) -C $(LWIP_DIR)/proj/aros

$(PKTDRV_LIBDIR)/libpktdrv.a:
	$(MAKE) -C $(PKTDRV_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f apps/*.o
	rm -f main.exe
