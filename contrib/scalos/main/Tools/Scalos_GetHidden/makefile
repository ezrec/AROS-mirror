# ECX makefile for Amigas/68k GetHidden tool
# $Date$
# $Revision$
# $Id$

#############################################################

SUBDIRMAKE 	= $(MAKE) -s -C 

#############################################################


.PHONY: clean install nodebug

#############################################################

CC      	= ECX:bin/ECX

CFLAGS  	= AMIGAOS VAR LINE SYM ICA

OPTFLAGS  	= EXENAME

#############################################################

CATCOMP 	= CatComp
CAT_FILE	= Scalos/GetHidden.catalog
DESTCAT		= Locale:Catalogs

CATS		= deutsch \
		  français

ALLCATS		= $(foreach cat,$(CATS),catalogs/$(cat)/$(CAT_FILE))

#############################################################

NAME		= gethidden.68k
DESTTOOL	= Scalos:Tools/GetHidden

#############################################################

SRC  		= gethidden.e

MSR2		= Scalos_GetHidden_Locale.e
MOD2		= Scalos_GetHidden_Locale.m

MSR3		= getpath.e
MOD3		= getpath.m

MSR4		= GetIconObject.e
MOD4		= GetIconObject.m

MSR5		= MyDebugOnOff.e
MOD5		= MyDebugOnOff.m

MSR6		= TCPHeaders.e
MOD6		= TCPHeaders.m

MSR7		= GetTCP.e
MOD7		= GetTCP.m

MSR8		= OnlineUpdate.e
MOD8		= OnlineUpdate.m

MSR9		= VERSTAGECX.e
MOD9		= VERSTAGECX.m

#############################################################

All:	$(MOD2) $(MOD3) $(MOD4) $(MOD5) $(MOD6) $(MOD7) $(MOD8) $(MOD9) $(NAME) allcatalogs
	

#############################################################

# commands generated a catalog (.catalog) from a catalog translation (.ct) file.
.ct.catalog:
	$(CATCOMP) $*.cd $< CATALOG $*.catalog VB=1

$(MOD2):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR2) $(CFLAGS)

$(MOD3):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR3) $(CFLAGS)

$(MOD4):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR4) $(CFLAGS)
$(MOD5):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR5) $(CFLAGS)
$(MOD6):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR6) $(CFLAGS)
$(MOD7):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR7) $(CFLAGS)
$(MOD8):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR8) $(CFLAGS)
$(MOD9):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(MSR9) $(CFLAGS)

$(NAME):
	@printf '\033[32mCompile: \033[31m\033[1m$@\033[0m\n'
	@$(CC) $(SRC) $(CFLAGS) $(OPTFLAGS) $@

############################################################

# make all GetHidden .catalogs
allcatalogs:
	-@$(foreach cat,$(CATS),$(SUBDIRMAKE) catalogs/$(cat)/Scalos;)

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mDelete: \033[31m\033[1m'
	@delete $(MOD2) $(MOD3) $(MOD4) $(MOD5) $(MOD6) $(MOD7) $(MOD8) $(MOD9) $(NAME) $(ALLCATS)
	@printf '\033[0m'

#############################################################

# copy all generated file to their final destinations
install:
	@printf '\033[32mInstall: \033[31m\033[1m$(DESTTOOL)\033[0m\n'
	@copy $(NAME) $(DESTTOOL) clone
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@$(foreach cat,$(CATS),copy "catalogs/$(cat)/$(CAT_FILE)" "$(DESTCAT)/$(cat)/Scalos" clone;)


