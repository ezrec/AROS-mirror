LOCAL_FILES  := $(wildcard Lib/test/*.py) $(wildcard Lib/test/*.uue) $(wildcard Lib/test/test.*)

MODULE_DIRS  += test
MODULE_FILES += test/README $(patsubst Lib/%, %, $(LOCAL_FILES))

include Lib/test/output/module.make
