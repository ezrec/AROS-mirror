LOCAL_FILES := \
    	__init__.py \
    	domreg.py \
    	minidom.py \
    	pulldom.py \

MODULE_DIRS  += xml/dom
MODULE_FILES += $(patsubst %, xml/dom/%, $(LOCAL_FILES))
 
