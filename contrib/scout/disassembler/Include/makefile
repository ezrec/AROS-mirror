FILES := proto/disassembler.h \
	 defines/disassembler.h inline/disassembler.h ppcinline/disassembler.h \
	 lvo/disassembler_lvo.i pragmas/disassembler_pragmas.h

DEPS := disassembler_lib.fd clib/disassembler_protos.h

all: $(FILES)

proto/disassembler.h: $(DEPS)
	mkdir -p proto
	fd2inline --mode=proto $^ -o $@

defines/disassembler.h: $(DEPS)
	mkdir -p defines
	fd2inline --mode=new --target=x-aros $^ -o $@

inline/disassembler.h: $(DEPS)
	mkdir -p inline
	fd2inline --mode=new $^ -o $@

ppcinline/disassembler.h: $(DEPS)
	mkdir -p ppcinline
	fd2pragma -i disassembler_lib.fd -c clib/disassembler_protos.h -s 130 -t ppcinline

lvo/disassembler_lvo.i: $(DEPS)
	mkdir -p lvo
	fd2pragma -i disassembler_lib.fd -c clib/disassembler_protos.h -s 20 -t lvo

pragmas/disassembler_pragmas.h: $(DEPS)
	mkdir -p pragmas
	fd2pragma -i disassembler_lib.fd -c clib/disassembler_protos.h -s 6 -t pragmas
	mv pragmas/disassembler_lib.h $@

clean:
	rm -rf proto defines inline ppcinline lvo pragmas
