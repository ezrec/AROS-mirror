SHELL=	/bin/sh
ED=	$(objdir)/ed
TMP=	$(objdir)/ed-test

all: check
	@:

check: build test
	@cd $(TMP); \
	grep '\*\*\*' *.ck  | sed 's/^[^*]*//'; \
	if grep '\*\*\*' *.ck  >/dev/null; then :; else \
		echo "tests completed successfully."; \
	fi

test: build ckscripts.sh
	@echo testing $(ED) ...
	@./ckscripts.sh $(ED) $(TMP)/

build: mkdirs mkscripts.sh
	@if test -f $(TMP)/errs.ck ; then :; else \
		echo "building test scripts for $(ED) ..."; \
		./mkscripts.sh $(ED) $(TMP)/; \
	fi

mkdirs:
	@$(objdir)/mkinstalldirs $(TMP)

distclean: clean
	@:

clean:
	rm -rf $(TMP)
