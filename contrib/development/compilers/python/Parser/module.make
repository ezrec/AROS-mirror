LOCAL_FILES := \
	acceler \
	bitset \
	firstsets \
	grammar \
	grammar1 \
	listnode \
	metagrammar \
	myreadline \
	node \
	parser \
	parsetok \
	printgrammar \
	tokenizer \
	intrcheck

USER_CFLAGS += -IParser
FILES       += $(patsubst %, Parser/%, $(LOCAL_FILES))
