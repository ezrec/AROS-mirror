
CC = gcc

CFLAGS = -w -O3 -m68020 -m68881 -DHAVE64BIT

LDFLAGS= -s -noixemul
LIBS = -lm -lauto -lmui
OBJEXT = o
OBJS= atak.$(OBJEXT) book.$(OBJEXT) \
	epd.$(OBJEXT) eval.$(OBJEXT) genmove.$(OBJEXT) hash.$(OBJEXT) \
	hung.$(OBJEXT) init.$(OBJEXT) iterate.$(OBJEXT) main.$(OBJEXT) \
	modules.$(OBJEXT) move.$(OBJEXT) mui_chess.$(OBJEXT) mui_board.$(OBJEXT) mui_field.$(OBJEXT) null.$(OBJEXT) \
	players.$(OBJEXT) pgn.$(OBJEXT) quiesce.$(OBJEXT) \
	random.$(OBJEXT) repeat.$(OBJEXT) search.$(OBJEXT) \
	solve.$(OBJEXT) sort.$(OBJEXT) swap.$(OBJEXT) \
	ttable.$(OBJEXT) util.$(OBJEXT) vars.$(OBJEXT)

%.$(OBJEXT): %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

AmiChess: $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

atak.$(OBJEXT): atak.c common.h
book.$(OBJEXT): book.c common.h
epd.$(OBJEXT): epd.c common.h
eval.$(OBJEXT): eval.c common.h eval.h
genmove.$(OBJEXT): genmove.c common.h
hash.$(OBJEXT): hash.c common.h
hung.$(OBJEXT): hung.c common.h
init.$(OBJEXT): init.c common.h
iterate.$(OBJEXT): iterate.c common.h
main.$(OBJEXT): main.c common.h
modules.$(OBJEXT): modules.c
move.$(OBJEXT): move.c common.h
mui_chess.$(OBJEXT): mui_chess.c common.h
mui_board.$(OBJEXT): mui_board.c common.h
mui_field.$(OBJEXT): mui_field.c common.h
null.$(OBJEXT): null.c common.h
pgn.$(OBJEXT): pgn.c common.h
players.$(OBJEXT): players.c common.h
quiesce.$(OBJEXT): quiesce.c common.h
random.$(OBJEXT): random.c common.h
repeat.$(OBJEXT): repeat.c common.h
search.$(OBJEXT): search.c common.h
solve.$(OBJEXT): solve.c common.h
sort.$(OBJEXT): sort.c common.h
swap.$(OBJEXT): swap.c common.h
ttable.$(OBJEXT): ttable.c common.h
vars.$(OBJEXT): vars.c common.h

