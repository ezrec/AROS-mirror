/*
*	type definition for key used in remember-memory routines.
*	MWS, March 20, 1991.
*/

typedef struct remember {
	struct remember *next;
	void *ptr;
} RemKey;

/* Prototypes for functions defined in memory.c */
void *emalloc(unsigned int n);
RemKey **rem_setkey(RemKey **);
void *rem_malloc(unsigned int size);
void rem_freeall(void);
void rem_freelinks(void);
