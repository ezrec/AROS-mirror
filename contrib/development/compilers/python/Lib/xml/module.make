LOCAL_FILES := \
    	__init__.py

MODULE_DIRS  += xml
MODULE_FILES += $(patsubst %, xml/%, $(LOCAL_FILES))

include Lib/xml/parsers/module.make
include Lib/xml/dom/module.make
include Lib/xml/sax/module.make
