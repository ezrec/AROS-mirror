# $Id: Makefile,v 1.1 2005/08/04 20:02:29 itix Exp $

CC = gcc -noixemul
CFLAGS = -O3 -Wall -I. -Iinclude
TARGET = gtk-mui.library

OBJS  = Library.o

ECHO = echo
ECHE = echo -e
BOLD = \033[1m
NRML = \033[22m

COMPILING = @$(ECHE) "compiling $(BOLD)$@$(NRML)..."
LINKING = @$(ECHE) "linking $(BOLD)$@$(NRML)..."
STRIPPING = @$(ECHE) "stripping $(BOLD)$@$(NRML)..."
ARCHIVING = @$(ECHE) "archiving $(BOLD)$@$(NRML)..."

%.o: %.c
	$(COMPILING)
	@$(CC) $(CFLAGS) -o $@ -c $*.c

all: $(OBJS) libgtk-mui.a
	$(LINKING)
	@$(CC) -nostartfiles $(OBJS) -o $(TARGET).db -L.. -lgtk-mui
	$(STRIPPING)
	strip --remove-section=.comment $(TARGET).db -o $(TARGET)

headers:
	@-mkdir -p include/proto include/ppcinline include/inline
	cvinclude.pl --fd include/fd/gtk-mui_lib.fd --clib include/clib/gtk-mui_protos.h --proto include/proto/gtk-mui.h --inlines include/ppcinline/gtk-mui.h --vbccinlines include/inline/gtk-mui_protos.h --root=gtkmui

libgtk-mui.a: libgtk-mui.o libgtk-mui_glue.a
	$(ARCHIVING)
	@-rm -f libgtk-mui.a
	@cp libgtk-mui_glue.a libgtk-mui.a
	@$(AR) cru libgtk-mui.a libgtk-mui.o
	@ppc-morphos-ranlib libgtk-mui.a

libgtk-mui_glue.a: include/clib/gtk-mui_protos.h include/fd/gtk-mui_lib.fd
	$(COMPILING)
	@cvinclude.pl --fd=include/fd/gtk-mui_lib.fd --clib=include/clib/gtk-mui_protos.h --gluelib=libgtk-mui_glue.a

clean: 
	rm -f *.o $(TARGET) *.library include/proto/* include/ppcinline/* include/inline/*
	rmdir include/proto include/ppcinline include/inline
