#include <exec/lists.h>

struct node {
    struct Node node;
    DSTR  data;
};


struct carrier {
    struct Node node;
    struct List nodes;

    int   type;
    unsigned long is_const :  1;
    unsigned long flags    : 31;
};

extern struct carrier *current;

extern struct List carriers;

void carrier_name (struct carrier *c, char * name);
void carrier_type (struct carrier *c, int typ);
void carrier_const (struct carrier *c);
void carrier_cpy (struct carrier *dest, struct carrier *c);
void carrier_drop (struct carrier *c);
struct carrier * carrier_lock (char *name);
struct carrier * carrier_new (void);
void node_paste (struct node *n, char *data);
struct node * node_lock (char *name);
void node_add (struct carrier *c, char *name, char *data);
void node_drop (struct node *n);
void node_free (struct node *n);

#define CT_FNC	0
#define CT_STR	1
#define CT_INT	2
#define CT_BIT	3
#define CT_TREE 4
#define CT__MAX 4

extern struct carrier  defaults[CT__MAX + 1];
extern struct List     Carriers[CT__MAX + 1];

