# ECX makefile for 68k opendrawer cli tool
# $Date$
# $Revision$
# $Id$
#############################################################

CC      	= ECX:bin/ECX

CFLAGS  	= AMIGAOS VAR LINE SYM ICA

OPTFLAGS  	= EXENAME

#############################################################

NAME		= opendrawer.68k

#############################################################

SRC  		= opendrawer.e

MODSRC		= verstag.e

MOD		= verstag.m

#############################################################

DESTTOOL	= Scalos:Tools/Opendrawer

#############################################################

All: 	$(MOD) $(NAME)

#############################################################

$(MOD):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MODSRC) $(CFLAGS)

$(NAME):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(SRC) $(CFLAGS) $(OPTFLAGS) $@

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mDelete: \033[31m\033[1m'
	@delete $(NAME) $(MOD)
	@printf '\033[0m'

#############################################################

# copy all generated file to their final destinations
install:
	@printf '\033[32mInstall:\033[31m\033[1m $(NAME) to $(DESTTOOL)\033[0m\n'
	@copy $(NAME) $(DESTTOOL) clone

