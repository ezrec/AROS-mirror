LOCAL_FILES := \
	abstract \
	bufferobject \
	cellobject \
	classobject \
	cobject \
	complexobject \
	dictobject \
	fileobject \
	frameobject \
	funcobject \
	intobject \
	listobject \
	longobject \
	methodobject \
	moduleobject \
	object \
	rangeobject \
	sliceobject \
	stringobject \
	tupleobject \
	typeobject \
	floatobject \
	unicodectype \
	unicodeobject 

# Don't compile obmalloc unless WITH_PYMALLOC is defined, or we'll crash 
# and burn (since then _THIS_MALLOC == _SYSTEM_MALLOC in obmalloc then
# --> shadows clib's malloc --> infinite recursion --> segfault.
# Since obmalloc basically defines memory pool oriented memory allocation
# for speed reasons and AROS' malloc uses memory pools, it's probably
# not needed. Investigate further...

USER_CFLAGS += -IObjects
FILES       += $(patsubst %, Objects/%, $(LOCAL_FILES))
