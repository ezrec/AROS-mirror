GETOPT = 
#GETOPT = getopt.o

unsit : unsit.o updcrc.o $(GETOPT)
	cc -o unsit unsit.o updcrc.o $(GETOPT)

unsit.o : unsit.c stuffit.h
getopt.o : getopt.c

unsit.shar : README Makefile unsit.c stuffit.h updcrc.c getopt.c unsit.1
	shar README Makefile unsit.c stuffit.h updcrc.c getopt.c unsit.1 \
	>unsit.shar
