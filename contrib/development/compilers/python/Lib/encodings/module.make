MODULE_DIRS  += encodings
MODULE_FILES += $(patsubst Lib/%, %, $(wildcard Lib/encodings/*.py))
