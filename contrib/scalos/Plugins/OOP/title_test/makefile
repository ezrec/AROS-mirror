# makefile for title_test.plugin
# $Date$
# using SAS/C

#############################################################

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

CHEADERS 	= 
CC      	= sc
CFLAGS  	= nostkchk nochkabort strcons strmer opt dbg=ff nover \
		  streq data=far idir=sc:include/ idir=include: \
		  idlen=64 idir=///include
AS      	= phxass
AFLAGS  	= quiet m=68020 linedebug opt=NRQB i=SC:Assembler_Headers
LD      	= slink
ECHO		= echo
LDFLAGS 	= quiet batch noicons sd
LDLIBS  	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
CSTARTUP 	=
OBJDIR		= .sasobj

#############################################################

PLUGNAME = title_test.plugin

#############################################################

all:	$(PLUGNAME)

#############################################################

CSRCS 	= 
ASRCS	=	title_test.s

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

OBJS = $(ASRCS:%.s=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)

#############################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile $<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble $<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble $<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#############################################################

$(PLUGNAME): $(OBJS) 
	@printf '\033[32mLink $@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $(PLUGNAME) LIB $(LDLIBS) $(DBFLAG) $(LDFLAGS) stripdebug
	@$(LD) FROM $(OBJS) TO $(PLUGNAME).debug LIB $(LDLIBS) $(DBFLAG) $(LDFLAGS) addsym

#############################################################

$(OBJDIR)/title_test : scalos_macros.i

#############################################################

# A little something to clean it all up
clean:
	-@delete $(OBJS) $(PLUGNAME) $(PLUGNAME).debug

#############################################################

install:
	copy $(PLUGNAME) Scalos:Plugins/OOP clone

#############################################################
