
AUTOMAKE_OPTIONS = no-dependencies 1.1g

SUBDIRS = amiga atari human68k mac msdos nt os2 ram tops20 unix vms

EXTRA_DIST = BUGS ChangeLog.amiga History.512 ToDo Where ZipPorts \
	Contents Makefile.gccamiga README.amiga

bin_PROGRAMS = unzip funzip unzipsfx

gccAmigaOS_special = -DUNIX -DBSD -DTERMIOS -UAMIGA -DCHECK_EOF

DEFS = @DEFS@ $(gccAmigaOS_special)
INCLUDES = -I$(srcdir) -I.


## WATCH: AmigaOS may replace unix.o with amiga.o
## I hope the next rule works.  Using unzip_DEPENDENCIES and unzip_LDADD
## worls fine, but only automake-1.1e generates a usabe Makefile.in.
## After the update to automake-1.1g I'm forced to change this rules to make
## c-source-code visible to force automake-1.1g to ouput the definition
## of the COMPILE command.
unzip_SOURCES = unzip.c crypt.c envargs.c explode.c extract.c \
	file_io.c inflate.c match.c unreduce.c unshrink.c zipinfo.c unix.c
##unzip_DEPENDENCIES = unzip.o crypt.o envargs.o explode.o extract.o \
	file_io.o inflate.o match.o unreduce.o unshrink.o zipinfo.o unix.o
##unzip_LDADD = unzip.o crypt.o envargs.o explode.o extract.o \
	file_io.o inflate.o match.o unreduce.o unshrink.o zipinfo.o unix.o

funzip_SOURCES = # funzip.c crypt_.c inflate_.c
funzip_DEPENDENCIES = funzip.o crypt_.o inflate_.o
funzip_LDADD = funzip.o crypt_.o inflate_.o

unzipsfx_SOURCES = # unzipsfx.c crypt.c extract_.c file_io.c inflate.c match.c unix_.c
unzipsfx_DEPENDENCIES = unzipsfx.o crypt.o extract_.o file_io.o inflate.o match.o unix_.o
unzipsfx_LDADD = unzipsfx.o crypt.o extract_.o file_io.o inflate.o match.o unix_.o


## unzip objects
unzip.o : $(srcdir)/unzip.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/version.h config.h

crypt.o : $(srcdir)/crypt.c $(srcdir)/unzip.h $(srcdir)/zip.h $(srcdir)/crypt.h config.h

envargs.o : $(srcdir)/envargs.c $(srcdir)/unzip.h config.h

explode.o : $(srcdir)/explode.c $(srcdir)/unzip.h config.h

extract.o : $(srcdir)/extract.c $(srcdir)/unzip.h $(srcdir)/crypt.h config.h

file_io.o : $(srcdir)/file_io.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/tables.h config.h

inflate.o : $(srcdir)/inflate.c $(srcdir)/inflate.h $(srcdir)/unzip.h config.h

match.o : $(srcdir)/match.c $(srcdir)/unzip.h config.h

unreduce.o : $(srcdir)/unreduce.c $(srcdir)/unzip.h config.h

unshrink.o : $(srcdir)/unshrink.c $(srcdir)/unzip.h config.h

zipinfo.o : $(srcdir)/zipinfo.c $(srcdir)/unzip.h config.h

unix.o : $(srcdir)/unix/unix.c $(srcdir)/unzip.h config.h
	$(COMPILE) -c $(srcdir)/unix/unix.c -o unix.o
 

## funzip special objects
crypt_.o : $(srcdir)/crypt.c $(srcdir)/unzip.h $(srcdir)/zip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DFUNZIP -c $(srcdir)/crypt.c -o crypt_.o

inflate_.o : $(srcdir)/inflate.c $(srcdir)/inflate.h $(srcdir)/unzip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DFUNZIP -c $(srcdir)/inflate.c -o inflate_.o

## unzipsfx special objects
unzipsfx.o : $(srcdir)/unzip.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/version.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/unzip.c -o unzipsfx.o

extract_.o : $(srcdir)/extract.c $(srcdir)/unzip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/extract.c -o extract_.o

unix_.o : $(srcdir)/unix/unix.c $(srcdir)/unzip.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/unix/unix.c -o unix_.o
