#		requires djgpp(2.0) and allegro(2.1)

#for normal GCC
CFLAGS1= -m486 -O2 -fstrength-reduce -ffast-math -fforce-mem -fforce-addr -fomit-frame-pointer -I ../../../../../aalib-1.2  -L.  -D DOG_DRIVER  -Wall
#for PGCC
#CFLAGS1= -mpentium -O2 -fstrength-reduce -ffast-math -fforce-mem -fforce-addr -fomit-frame-pointer -fno-exceptions -fmove-all-movables -I ../../../../../aalib-1.2  -L.  -D DOG_DRIVER  -Wall
LIBS = -lalleg -L ../../vga -L ../../text -L ../../aalib-1.2 -laa -ltext -lvga -lpng -lz -lm
LFLAGS = -s

