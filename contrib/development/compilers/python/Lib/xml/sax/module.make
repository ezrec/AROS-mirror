LOCAL_FILES := \
    	__init__.py \
    	_exceptions.py \
    	expatreader.py \
    	handler.py \
    	saxutils.py \
    	xmlreader.py

MODULE_DIRS  += xml/sax
MODULE_FILES += $(patsubst %, xml/sax/%, $(LOCAL_FILES))
