

#define timeval arostimeval
#include <exec/types.h>
#include <exec/lists.h>

#include <proto/alib.h>
#include <proto/exec.h>
#undef timeval

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Dyn.h"
/* #include <alloca.h> */
/* #include <lists.h> */

#include "main.h"

BOOL flexprintf (void* outstream, int (*writestring)(void*, char *), char * tmplt, void* instream, char * (*getstring)(void*, char *));




struct carrier *current  = NULL;
struct List	       carriers;

struct carrier	defaults[CT__MAX + 1];
struct List	Carriers[CT__MAX + 1];


/* ************************************************** */


void node_free (struct node *n) {
    if (n->node.ln_Name)
	free(n->node.ln_Name);
    DynClear(&n->data);
    free    (n);
} /* node_free */


void node_drop (struct node *n) {
    REMOVE (&n->node);
    node_free(n);
} /* node_drop */

void node_add (struct carrier *c, char *name, char *data) {
    struct node *n;
    if ((n = malloc(sizeof(*n)))) {
	bzero(n, sizeof (*n));
	DynSet(&n->data, 1, data);
	n->node.ln_Name = name;
	ADDHEAD(&c->nodes, &n->node);
    } /* if */
    assert(n != NULL);
} /* node_add */

struct node * node_lock (char *name) {
    struct node *n;
    for (n = GetHead(&current->nodes); n; n = GetSucc(n))
	if (strcasecmp (n->node.ln_Name, name) == 0)
	    return n;
    return NULL;
} /* node_lock */

void node_paste (struct node *n, char *data) {
    DynAppend(&n->data, 1, data);
} /* node_paste */


/* ************************************************** */


struct carrier * carrier_new (void) {
    struct carrier *c;

    if ((c = malloc (sizeof(*c)))) {
	bzero(c, sizeof (*c));

	ADDHEAD(&carriers, &c->node);
	current = c;
	NEWLIST(&c->nodes);
    } /* if */
    assert (c);
    return c;
} /* carrier_new */

struct carrier * carrier_lock (char *name) {
    struct carrier *c;
    for (c = GetHead(&carriers); c; c = GetSucc(c))
	if (strcmp (c->node.ln_Name, name) == 0)
	    return c;
    return NULL;
} /* carrier_lock */

void carrier_drop (struct carrier *c) {

    if	((c == &defaults[0]) || (c == &defaults[1]) || (c == &defaults[2]) || (c == &defaults[3]))
	return;

    REMOVE (&c->node);

    if (c->node.ln_Name);
	free (c->node.ln_Name);

    {
	struct node *n, *nn;
	for (n = GetHead(&c->nodes); n; n = nn) {
	    nn = GetSucc(n);
	    node_free(n);
	} /* for */
    }

    free (c);
} /* carrier_drop */


/* !achtung, der name wird _NICHT_ kopiert! */
void carrier_cpy (struct carrier *dest, struct carrier *c) {
    struct node *n;
    for (n = GetHead(&c->nodes); n; n = GetSucc(n)) {
	node_add(dest, strdup(n->node.ln_Name), DynValue(&n->data));
    } /* for */
} /* carrier_cpy */


void carrier_const (struct carrier *c) {
    c->is_const = 1;
} /* carrier_const */

void carrier_type (struct carrier *c, int typ) {
    c->type = typ;
    carrier_cpy(c, &defaults[typ]);
} /* carrier_type */

void carrier_name (struct carrier *c, char * name) {
    /* struct carrier *l; if (l = carrier_lock(name)){carrier_drop(current); current = l;} else */
    c->node.ln_Name = strdup(name);
} /* carrier_name */


/* ************************************************** */


void prepare (void) {
    /* sortieren der eintraege */

    struct carrier *c, *m;
    struct List inter;

    NEWLIST (&inter);

fprintf (stderr, "sorting\n");

    while (m = GetHead(&carriers)) {
	if (!m->node.ln_Name) {
	    REMOVE(&m->node);
	    continue;
	}
	for (c = GetSucc(m); c; c = GetSucc(c))
	    if (strcmp(m->node.ln_Name, c->node.ln_Name) < 0)
		m = c;
	REMOVE (&m->node);
	ADDHEAD(&inter, &m->node);
    }

    while (c = GetHead(&inter)) {
	REMOVE (&c->node);
	if (c->type != CT_TREE)
	    ADDHEAD(&carriers, &c->node);
	else
	    ADDHEAD(&Carriers[CT_TREE], &c->node);
    }
} /* prepare */


/* ************************************************** */


int query = 0;
char *getvalue(struct carrier *c, char *name) {
    struct node *n;
    current = c;

    if (!strcasecmp(name, "name")) {
	query = 1;
	return c->node.ln_Name;
    }

/* fprintf (stderr, "searching '%s'\n", name); */
    if (n = node_lock (name)) {
	query = 1;
	return DynValue(&n->data);
    }
    current = &defaults[c->type];
    if (n = node_lock (name)) {
	query = 1;
	return DynValue(&n->data);
    }

fprintf (stderr, "FS:[%s/%s]\n", c->node.ln_Name, name);
    assert (name == "search failed");

    return NULL;
}


char *expand (char *tplt, struct carrier *c) {
    DSTR buffer = EmptyDyn;
    char * b2;
    int loops = 5;

    b2 = strdup(tplt);
    assert (b2);
    do {
	query = 0;
	if (!flexprintf (&buffer, DynApp1, b2, c, getvalue))
	    assert(NULL);
	free (b2);
	b2 = strdup(DynValue(&buffer));
	assert(b2);
	DynClear(&buffer);
    } while (query && --loops);

    return b2;
}




void output (char *name) {
    struct carrier *c;
    struct node    *n;
    int 	    i;
    FILE *fi;
    char *bf;

fprintf (stderr, "sending\n");

    if (!(fi = fopen (name, "w"))) {
	fprintf (stderr, "FILE IO error\n");
	exit(-1);
    }

    fprintf (fi,     "const char *spc_names[] = {\n");
    for (i = 0, c = GetHead(&carriers); c; c = GetSucc(c), ++i) {
	fprintf (fi, "\t/* %3d */ \"%s\",\n", i, c->node.ln_Name);
    }
    fprintf (fi,     "};\n"
		     "\n"
		     "#define spc_num_vars %d\n"
		     "\n", i);

    fprintf (fi, "char *spc_get(long lock) {\n"
		 "    long array[6];\n"
		 "    char *value   = NULL;\n"
		 "    char *av0_bak = av[0];\n"
		 "    char *av1_bak = av[1];\n"
		 "    char *av2_bak = av[2];\n"
		 "    switch (lock) {\n" );
    for (i = 0, c = GetHead(&carriers); current = c; c = GetSucc(c), ++i) {
	if ((n = node_lock("GET")) || (current = &defaults[c->type]) && (n = node_lock("GET"))) {
	    bf = expand ("$(GET)", c);
	    fprintf (fi, "    case ~%d: /* %s */\n"
			 "%s\n"
			 "    break;\n", i, c->node.ln_Name, bf);
	    free(bf);
	} else
	    fprintf (fi, "#warning \" no GET for %s\"\n", c->node.ln_Name);
    }
    fprintf (fi,         "    default:\n"
			 "        return NULL;\n"
			 "    }\n"
			 "    return value;\n"
			 "} /* spc_get */\n"
			 "\n"
			 "\0********************************************");


    fprintf (fi, "int spc_set(long lock, char *value) {\n"
		 "    long array[6];\n"
		 "    char *av0_bak = av[0];\n"
		 "    char *av1_bak = av[1];\n"
		 "    char *av2_bak = av[2];\n"
		 "    switch (lock) {\n" );
    for (i = 0, c = GetHead(&carriers); current = c; c = GetSucc(c), ++i) {
	if (c->is_const)
	    continue;
	if ((n = node_lock("SET")) || (current = &defaults[c->type]) && (n = node_lock("SET"))) {
	    bf = expand ("$(SET)", c);
	    fprintf (fi, "    case ~%d: /* %s */\n"
			 "%s\n"
			 "    break;\n", i, c->node.ln_Name, bf);
	    free(bf);
	} else
	    fprintf (fi, "#warning \" no SET for %s\"\n", c->node.ln_Name);
    }
    fprintf (fi,         "    default:\n"
			 "        return 0;\n"
			 "    }\n"
			 "    return 1;\n"
			 "} /* spc_set */\n"
			 "\n"
			 "\0********************************************");

    fclose (fi);
} /* output */




/* ************************************************** */


int main (int ac, char **av) {
    extern int yy_flex_debug;
    extern int yydebug;

    yy_flex_debug = 0;
    yydebug	  = 0;
    if (ac >= 2 && av[1][0] == '-' && av[1][1] == 'd') {
	yydebug       = 1;
	yy_flex_debug = 1;
    }

    NEWLIST(&carriers);
    {
	int i;
	for (i = 0; i <= CT__MAX; ++i) {
	    NEWLIST(&defaults[i].nodes);
	    NEWLIST(&Carriers[i]);

	}
    }

    carrier_new();

    {
	extern int yyparse(void);
	yyparse();
    }

    prepare();

    if (ac > 1)
	output(av[ac-1]);

}


