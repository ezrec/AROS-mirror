MODULE_DIRS  += test/output
MODULE_FILES += $(patsubst Lib/%, %, $(wildcard Lib/test/output/test_*))
