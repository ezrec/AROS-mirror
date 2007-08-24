EXTENS = fc

include Feelin:Sources/makefile.header

objs := $(patsubst %.c,%.o,$(wildcard *.c))

$(DST)/$(NAME).$(EXTENS) : $(objs)
	@ Avail >NIL: FLUSH
	@ $(LINK) $(LOPT) FROM Project.o $(filter-out Project.o, $^) TO $(DST)/$(NAME).$(EXTENS)
	@ echo ">>" [1m$(DST)/$(NAME).$(EXTENS)[0m Version [1m$(VER).$(REV)[0m

