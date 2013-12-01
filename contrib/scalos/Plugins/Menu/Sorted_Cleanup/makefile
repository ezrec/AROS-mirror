# makefile for Scalos sorted_cleanup.plugin
# $Date$

#############################################################

.SUFFIXES: .s .plugin .plugin.debug

#############################################################

ASRCS = sorted_cleanup.s
AS = phxAss
LD = slink
LDLIBS = LIB:debug.lib LIB:amiga.lib
PRECOMP = Include:all.gst
OBJDIR	= .sasobj

#############################################################

LNFLAGS	  = quiet batch noicons stripdebug
LNDBFLAGS = quiet batch noicons addsym

ifdef L
	AFLAGS = quiet DS NOEXE opt=NRQB LIST=* linedebug I=SC:Assembler_Headers
else
	AFLAGS = quiet DS NOEXE opt=NRQB linedebug I=SC:Assembler_Headers
endif

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

OBJS = $(ASRCS:%.s=$(OBJDIR)/%.o)

#############################################################

NAME    = sorted_cleanup.plugin
DBGNAME = $(NAME).debug

#############################################################

all: 	$(NAME) $(DBGNAME)
# install
# clean

#############################################################

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#############################################################

$(NAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) to $@ LIB $(LDLIBS) $(LNFLAGS)

$(DBGNAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) to $@ LIB $(LDLIBS) $(LNDBFLAGS)

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME)\033[0m\n'
	@copy $(NAME) scalos:plugins/menu/ clone
	@avail flush

#############################################################

clean:
	@printf '\033[32mDelete: \033[1m\033[31m'
	@delete $(OBJS) $(NAME) $(DBGNAME)
	@printf '\033[0m'

#############################################################

