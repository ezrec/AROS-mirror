#define PATCH_AVL
/******************************************************************************

    MODUL
	varsbases.c

    DESCRIPTION
	lowlevel Variable support for DME/XDME
	(Basic List/AVL-Tree Functions)


    NOTES
	Either use AVLTREES or MLISTS

	For ALL commands (even to the searches) You must use
	the address of a tree/list, not the tree/list itself
	(that was done to simplify source-modification: just
	 replace 'List'/[M]LIST by 'Tree'/APTR, and go)

    BUGS
	<none known>

    TODO
	More homogene Interface - Use of an opaque type
	(So that silly redefinition in other modules can go away)

    EXAMPLES
	-/-

    SEE ALSO
	vars.c

    INDEX

    HISTORY
	<see RCS-File>


******************************************************************************/

/*
**  (C)Copyright 1992 by Bernd Noll for null/zero-soft
**  All Rights Reserved
**
**  RCS Header: $Id$
**
**  Basic Functions to work with trees/lists of DME-Variables
**  Put together to start a little bit of modularisation
**  in the sources.
**
*/

/**************************************
		Includes
**************************************/
#define TNODE struct TreeNode
#define VARS  struct _VARS
#define VBASE TNODE *

#include "AVL.h"
#include "defs.h"

/**************************************
	   Exports
**************************************/
/* --- You can only use either LISTS or AVL-Trees!!! */

/* Vars Operations on trees */
Prototype void	 DelAllVarsFromTree ( VBASE * tree );
Prototype void	 DelVarFromTree     ( VBASE * tree, char * name );
Prototype char * GetVarFromTree     ( VBASE * tree, char * name );
Prototype void	 SetVarIntoTree     ( VBASE * tree, char * name, char * value );


/* Lists Operations in Common */
Prototype char* names2string   ( MLIST* list, char* buffer, char delimiter, void* filter );  /* assumes buffer is 1/4k * BUFFERS chgars long */
Prototype int	numnodes       ( MLIST* list );
Prototype NODE* findnode       ( MLIST* list, char* name, short len );

/* Vars Operations on lists */
Prototype void	DelAllVarsFromList ( MLIST* list );
Prototype void	DelVarFromList ( MLIST* list, char* name );
Prototype char* GetVarFromList ( MLIST* list, char* name );
Prototype void	SetVarIntoList ( MLIST* list, char* name, char* value );




/**************************************
      Interne Defines & Strukturen
**************************************/
#ifdef PATCH_AVL
# ifdef TEST
#  define error printf
#  define nomemory()
# endif

struct _VARS
{
	TNODE  Node;
	char * Str;
};

#else

typedef struct _VARS
{
	NODE  Node;
	char * Str;
};

#define VBASE MLIST

#endif /* (not) PATCH_AVL */



/**************************************
      Implementation
**************************************/
#ifdef PATCH_AVL


/*
**  DelAllVarsFromTree()
**	recursively remove all nodes from a certain tree
**	and free them and their associated names&values
*/

void
DelAllVarsFromTree (VBASE * tree)
{
    if (!tree || !(*tree)) {
	return;
    } /* if */

    DelAllVarsFromTree(&(*tree)->tn_Left);        /* first free its sons */
    DelAllVarsFromTree(&(*tree)->tn_Right);

    free((*tree)->tn_Name);                       /* then free it and all its subs */
    free(((VARS*)*tree)->Str);
    free((*tree));

} /* DelAllVarsFromTree */


/*
**  SetVarIntoTree()
**	define a node with a cetain name and value in
**	a given tree if therew is already a node of the
**	chosen name the previous node is removed
**	(actually only the old value is removed)
**	if any allocation went wrong, call nomemory()
*/

void
SetVarIntoTree (VBASE* tree, char* name, char* value)
{
    VARS* v;
    char* vn = NULL;
    char* vv;

    if ((v = (VARS*)AVL_Find(tree, name)))
    {
	if ((vv = strdup(value)))
	{
	    free(v->Str);
	    v->Str = vv;
	    return;
	} /* if malloced */
	v = NULL; /* avoid freeing the found node */
    } else
    {
	v = malloc(sizeof(VARS));
	vv = strdup(value);
	vn = strdup(name);
	if (v && vv && vn)
	{
	    v->Str = vv;
	    v->Node.tn_Name = vn;
	    AVL_Append (tree, (TNODE *)v);
	    return;
	} /* if malloced */
    } /* if (no) old node */

    /* --- if we didn't return prior, (if everything worked fine) remove all allocated data */
    if (v)  free(v);
    if (vv) free(vv);
    if (vn) free(vn);
    nomemory();
} /* SetVarIntoTree */




/*
**  DelVarFromTree()
**	remove a certain node (found w/ FindTreeNode)
**	from a given tree; do not panik, if there is no
**	node found
*/

void
DelVarFromTree (VBASE* tree, char* name)
{
    VARS *v;

    if ((v = (VARS*)AVL_Find(tree, name)))
    {  /* if there is a node of the right name */
	AVL_Remove (tree, (TNODE *)v);         /* remove it and all its subs */
	free(v->Node.tn_Name);
	free(v->Str);
	free(v);
    } /* if */
} /* DelVarFromTree */





/*
**  GetVarFromTree()
**	find a node in a certain tree (w/ FindTreeNode)
**	duplicate its value and return the copy
**	return NULL if there was no node found
**	and call nomemory() if duplicate failed
*/

char*
GetVarFromTree (VBASE* tree, char* name)
{
    char* str;
    VARS* v;

    if ((v = (VARS*)AVL_Find(tree, name)))
    { /* if there is a node of the right name */
	if ((str = strdup(v->Str)))
	{		/* return a copy of its value */
	    return(str);
	} else
	{
	    nomemory();
	    return NULL;
	} /* if */
    } /* if */
    return(NULL);
} /* GetVarFromTree */



#ifdef TEST


void zeigebaum(TNODE* t)
{
    if (t==NULL)
    {
	printf( "-" );
	return;
    }

    printf("(%d%s ",t->tn_Balance,t->tn_Name);
    printf("L");
    zeigebaum(t->tn_Left);
    printf("R");
    zeigebaum(t->tn_Right);
    printf(")");
} /* zeigebaum */




char* testnamen[] =
{
    "prag",
    "lima",
    "tokio",
    "bonn",
    "paris",
    "sofia",
    "wien",
    "bern",
    "kairo",
    "oslo",
    "rom",
    "athen",
    NULL };



int main(int ac, char* av[])
{
    char** tt;
    char buffer [100];

    TNODE* staedte = NULL;
    TNODE* rem = NULL;

    for (tt = testnamen; *tt; tt++)
    {
	printf(">%s\n",*tt);
	SetVarIntoTree(&staedte, *tt, "");
/* gets      (buffer); */
    } /* for */

    zeigebaum (staedte);
    rem = AVL_Find (&staedte, "bonn");
    printf ("\nrem == %08lx %s\n", rem, rem->tn_Name);

    AVL_Remove (&staedte, rem);


    zeigebaum(staedte);
    puts ("");
} /* main */






#endif /* TEST */


#else /* ! PATCH_AVL */


/*
**  GetVarFromList()
**	find a node in a certain list (w/ FindName)
**	duplicate its value and return the copy
**	return NULL if there was no node found
**	and call nomemory() if duplicate failed
*/

char*
GetVarFromList (VBASE* list, char* name)
{
    char* str = NULL;
    VARS* v   = NULL;

    if ((v = (VARS*)FindName((LIST*)list, name)))
    { /* if there is a node of the right name */
	if ((str = strdup(v->Str)))
	{		/* return a copy of its value */
	    return(str);
	} else
	{
	    nomemory();
	    abort(NULL);
	} /* if */
    } /* if */
    return(NULL);
} /* GetVarFromList */



/*
**  DelVarFromList()
**	remove a certain node (found w/ FindName)
**	from a given list; do not panik, if there is no
**	node found
*/

void
DelVarFromList (VBASE* list, char* name)
{
    VARS *v = NULL;

    if ((v = (VARS*)FindName((LIST*)list, name)))
    { /* if there is a node of the right name */
	Remove((NODE *)v);                        /* remove it and all its subs */
	free(v->Node.ln_Name);
	free(v->Str);
	free(v);
    } /* if */
} /* DelVarFromList */



/*
**  DelAllVarsFromList()
**	remove all nodes from a certain list
**	and free them and their associated names&values
*/

void
DelAllVarsFromList (VBASE* list)
{
    VARS* v = NULL;

    while ((v = (VARS*)RemHead((LIST*)list)))
    {	/* while there is an entry left in the list */
	free(v->Node.ln_Name);                  /* free it and all its subs */
	free(v->Str);
	free(v);
    } /* while */
} /* DelAllVarsFromList */



/*
**  SetVarIntoList()
**	define a node with a cetain name and value in
**	a given list if therew is already a node of the
**	chosen name the previous node is removed
**	(actually only the old value is removed)
**	if any allocation went wrong, call nomemory()
*/

void
SetVarIntoList (VBASE* list, char* name, char* value)
{
    VARS* v  = NULL;
    char* vn = NULL;
    char* vv = NULL;

#ifdef N_DEF
    /* --- First it is checked, if You have a Special variable */
    /*
    ** these checks may be object of some changes in near future
    ** - I do now think, that it would be quite better
    ** to introduce a new function SETSPC for special variables
    ** and to call "SETLOCAL <num>" to set a local flag instead of "SET i<num>"
    */
    if (SetAnyFlag(name, value) || SetSpecialInt(name, value)
    || SetSpecialVar(name, value) )
    {
	return();
    } /* if */
#endif /* N_DEF */

    /*
    **	that routine is a little bit longer than the old one,
    **	but we need not free all data of older entries any
    **	more, so memory fragmentation should be reduced a bit:
    **	if there's an older entry, simply replace its value
    **	by the new one, else create a full new entry.
    */
    if ((v = (VARS*)FindName((LIST*)list, name)))
    {
	if (vv = strdup(value))
	{
	    free(v->Str);
	    v->Str = vv;
	    /* --- it might be senseful moving the changed entry to startoflist */
	    /* Remove(v); */
	    /* AddHead((LIST*)list, v) */
	    return;
	} /* if malloced */
	v = NULL; /* avoid removing the found node */
    } else
    {
	v = malloc(sizeof(VARS));
	vv = strdup(value);
	vn = strdup(name);
	if (v && vv && vn)
	{
	    v->Str = vv;
	    v->Node.ln_Name = vn;
	    AddHead((LIST *)list, (NODE *)v);
	    return;
	} /* if malloced */
    } /* if (no) old node */

    /* --- if we didn't return prior, (if everything worked fine) remove all allocated data */
    if (v)  free(v);
    if (vv) free(vv);
    if (vn) free(vn);
    nomemory();
    abort2();
} /* SetVarIntoList */



/*
**	    Names 2 String
**  copies all ln_Name s of a given List into a String and returns it
**  (if filter != NULL the names are filtered before)
**  There _must_ be a buffer big enough to fit all the Strings:
**	definements in defs.h:
**  BUFFERS is the number of "LINES" reserved for buffer
**  and LINE_LENGTH (== 256) is the length of one such line
**  <exec/lists.h> should be included
**
**  name2string does _not_ allocate memory!!!!
**  currently filter MUST BE SET to NULL to guarantee compatibility to later versions!
**  list MUST BE VALID! (there is no check)
*/

char*
names2string (MLIST* list, char* buffer, char delimiter, char* (*filter)())
{
    struct Node* node;
    char*	 buf	  = buffer;
    int 	 i	  = 0;
    char	 delim[2] =
    {delimiter,0};
    /* char    inter[128]; */

    node = (NODE*)list->mlh_Head;
    while (node->ln_Succ && node != (NODE*)&list->mlh_Tail)
    {
	buffer = buf+i;
/* ************************************ Hier gehoert noch der filter hin, ich brauchte ihn nur bisher noch nicht, daher liess ich ihn weg * */
	if ((i += strlen(node->ln_Name)) < BUFFERS*LINE_LENGTH-1)
	{
	    strcpy(buffer, node->ln_Name);
	    if (i < BUFFERS*LINE_LENGTH-1)
	    {
		buffer = buf+i;
		strcpy(buffer, delim);
		i++;
	    } else
	    {
		return(buf);
	    } /* if */
	} else
	{
	    break;
	} /* if */
	node = node->ln_Succ;
    } /* while */

    *buffer = 0;
    return(buf);
} /* names2string */



/*
** numnodes() - return the number of entries in a list
**
**  list MUST BE VALID! (there is no check)
*/

int
numnodes(MLIST* list)
{
    register int    i = 0;
    register MNODE* n;

    for (n = list->mlh_Head; (n != NULL) && (n->mln_Succ != NULL); n = n->mln_Succ)
    {
	i++;
    } /* for */
    return(i);
} /* numnodes */



/*
** findnode() - find a certain node in a list
**
**  search may be done up to a certain strlength (own function) or via FindName
**
**  list MUST BE VALID! (there is no check)
*/

NODE*
findnode(MLIST* list, char* name, short len)
{
    register NODE* node;

    if (len)
    {
	for (node = (NODE*)list->mlh_Head; node && node->ln_Succ; node = node->ln_Succ)
	{
	    if (strncmp(node->ln_Name, name, len) == 0)
	    {
		return(node);
	    } /* if */
	} /* for */
    } else
    {
	return(FindName((LIST*)list, name));
    } /* if */
    return(NULL);
} /* findnode */


#endif /* (not) PATCH_AVL */
/******************************************************************************
*****  ENDE varsbases.c
******************************************************************************/

