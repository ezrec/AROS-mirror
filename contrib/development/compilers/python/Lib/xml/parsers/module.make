LOCAL_FILES := \
    	__init__.py \
    	expat.py

MODULE_DIRS  += xml/parsers
MODULE_FILES += $(patsubst %, xml/parsers/%, $(LOCAL_FILES))
