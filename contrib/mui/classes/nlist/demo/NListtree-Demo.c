/******************************************************************************\
********************************************************************************
***
***	NListtree.mcc	Copyright (c) 1999-2000 by Apocalypse Hard- and Software
***					Carsten Scholling
***
***	Example program for MUI class NListtree.mcc
***
***
***	Please do not use this "to be rewritten"-piece of software as an
***	information resource on how to write programs in an object oriented
***	MUI environment. It simply shows how to use some of the NListtree
***	functions.
***
***
********************************************************************************
\******************************************************************************/


/*
**	Includes
*/

#ifdef __AROS__
#define MUI_OBSOLETE
#endif

#ifdef __SASC
 #include <pragmas/muimaster_pragmas.h>
 #include <pragmas/intuition_pragmas.h>
 #include <pragmas/graphics_pragmas.h>
 #include <pragmas/exec_pragmas.h>

 #include <clib/muimaster_protos.h>
 #include <clib/exec_protos.h>
 #include <clib/alib_protos.h>
#else
 #ifdef __GNUC__
  #include <proto/muimaster.h>
  #include <proto/exec.h>
  #include <proto/intuition.h>  
  #include <clib/alib_protos.h>
 #endif
#endif

#include <exec/memory.h>
#include <exec/types.h>
#include <dos/dos.h>

#include <MUI/NListtree_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>
#include <libraries/mui.h>

#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef __AROS__
#define REG(x)
#define GNUCREG(x)
#define SAVEDS
#define ASM
#else
#include "Compiler.h"
#endif

#ifdef MYDEBUG
 #ifdef __APHASO__
  #include <Debug.h>
 #else
  #define bug kprintf
  #define D(x)
  void kprintf( UBYTE *fmt, ... );
 #endif
#else
 #define bug
 #define D(x)
#endif


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


/*
**	Do not use stack sizes below 8KB!!
*/
LONG __stack = 16384;


/*
**	MUI library base.
*/
struct Library *MUIMasterBase;
struct IntuitionBase *IntuitionBase;

struct MUI_NListtree_TreeNode *tntest;

/*
**	MUI objects.
*/
STATIC APTR		app, window,lt_nodes,
				tx_info1,	tx_info2,	tx_info3,
				sl_treecol,	st_string,
				bt_open,	bt_close,	bt_expand,	bt_collapse,
				bt_insert,	bt_remove,	bt_exchange,bt_rename,
				bt_move,	bt_copy,	bt_moveks,	bt_copyks,
				bt_find,	bt_parent,	bt_sort,	bt_getnr,
				bt_redraw,	bt_selected,bt_showtree,bt_seltogg,
				bt_test,	bt_test2,	bt_test3,	bt_test4;


/*
**	Sample tree structure.
*/
struct SampleArray {
	STRPTR	name;
	UWORD   flags;
};

STATIC const struct SampleArray sa[] =
{
	"comp", TNF_LIST | TNF_OPEN,
		"sys", TNF_LIST | TNF_OPEN,
			"amiga", TNF_LIST | TNF_OPEN,
				"misc", 0x8000,
			"mac", TNF_LIST,
				"system", 0x8000,

	"de", TNF_LIST | TNF_OPEN,
		"comm", TNF_LIST,
			"software", TNF_LIST,
				"ums", 0x8000,
		"comp", TNF_LIST | TNF_OPEN,
			"sys", TNF_LIST | TNF_OPEN,
				"amiga", TNF_LIST,
					"misc", 0x8000,
					"tech", 0x8000,
				"amiga", 0x8000,

	"sort test", TNF_LIST | TNF_OPEN,
		"a", 0,
		"x", TNF_LIST,
		"v", 0,
		"g", TNF_LIST,
		"h", 0,
		"k", TNF_LIST,
		"u", 0,
		"i", TNF_LIST,
		"t", 0,
		"e", TNF_LIST,
		"q", 0,
		"s", TNF_LIST,
		"c", 0,
		"f", TNF_LIST,
		"p", 0,
		"l", TNF_LIST,
		"z", 0,
		"w", TNF_LIST,
		"b", 0,
		"o", TNF_LIST,
		"d", 0,
		"m", TNF_LIST,
		"r", 0,
		"y", TNF_LIST,
		"n", 0,
		"j", TNF_LIST,


	"m", TNF_LIST,
		"i", TNF_LIST,
			"c", TNF_LIST,
				"h", TNF_LIST,
					"e", TNF_LIST,
						"l", TNF_LIST,
							"a", TNF_LIST,
								"n", TNF_LIST,
									"g", TNF_LIST,
										"e", TNF_LIST,
											"l", TNF_LIST,
												"o", 0,

	"end", TNF_LIST,
		"of", TNF_LIST,
			"tree", 0,

	"Sort Test 2", TNF_LIST,

	NULL, 0
};



#ifdef MORPHOS
#undef MUI_NewObject
#undef MUI_MakeObject

#define DoMethod(obj, tags...)				({ULONG _tags[] = { tags }; DoMethodA((obj), (APTR)_tags);})
#define DoSuperMethod(cl, obj, tags...)		({ULONG _tags[] = { tags }; DoSuperMethodA((cl), (obj), (APTR)_tags);})
#define CoerceMethod(cl, obj, tags...)		({ULONG _tags[] = { tags }; CoerceMethodA((cl), (obj), (APTR)_tags);})

Object *MUI_NewObject(char *classname, Tag tag1,...)
{
	ULONG mav[50];
	va_list tags;
	int i=0;

	mav[i] = tag1;
	if(mav[i] != TAG_DONE)
	{
		va_start(tags, tag1);
		mav[++i] = va_arg(tags, ULONG);
		i++;
		while(i<50)
		{
			mav[i] = va_arg(tags, ULONG);
			if(mav[i] == TAG_DONE) break;
			mav[++i] = va_arg(tags, ULONG);
			i++;
		}
		va_end(tags);
	}
	return(MUI_NewObjectA(classname, (struct TagItem *)&mav));
}

Object *MUI_MakeObject(long type, ...)
{
	ULONG mav[15];
	va_list tags;
	int i=0;

	va_start(tags, type);
	while(i<10)
	{
		mav[i] = va_arg(tags, ULONG);
		i++;
	}
	va_end(tags);

	return(MUI_MakeObjectA(type, (ULONG *)&mav));
}

#endif



/*
**	Prototypes and hooks.
*/

#ifdef __AROS__
#define INITHOOK(hookname,hookfunc) \
    STATIC struct Hook hookname = { {NULL, NULL}, HookEntry, (void *)hookfunc, NULL }
#else
#define INITHOOK(hookname,hookfunc) \
    STATIC struct Hook hookname = { {NULL, NULL}, (void *)hookfunc, NULL, NULL }
#endif
    
LONG SAVEDS ASM confunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_ConstructMessage *msg GNUCREG(a1) );
INITHOOK(conhook,confunc);

LONG SAVEDS ASM desfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_DestructMessage *msg GNUCREG(a1) );
INITHOOK(deshook,desfunc);

LONG SAVEDS ASM mtfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_MultiTestMessage *msg GNUCREG(a1) );
INITHOOK(mthook,mtfunc);

LONG SAVEDS ASM dspfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_DisplayMessage *msg GNUCREG(a1) );
INITHOOK(dsphook,dspfunc);

LONG SAVEDS ASM compfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_CompareMessage *msg GNUCREG(a1) );
INITHOOK(comphook,compfunc);

LONG SAVEDS ASM insertfunc( REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(inserthook,insertfunc);

LONG SAVEDS ASM exchangefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) );
INITHOOK(exchangehook,exchangefunc);

LONG SAVEDS ASM renamefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(renamehook,renamefunc);

LONG SAVEDS ASM movefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) );
INITHOOK(movehook,movefunc);

LONG SAVEDS ASM copyfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) );
INITHOOK(copyhook,copyfunc);

LONG SAVEDS ASM moveksfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) );
INITHOOK(movekshook,moveksfunc);

LONG SAVEDS ASM copyksfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) );
INITHOOK(copykshook,copyksfunc);

LONG SAVEDS ASM findnamefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(findnamehook,findnamefunc);

LONG SAVEDS ASM sortfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(sorthook,sortfunc);

LONG SAVEDS ASM getnrfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(getnrhook,getnrfunc);

LONG SAVEDS ASM numselfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(numselhook,numselfunc);

LONG SAVEDS ASM testfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) );
INITHOOK(testhook,testfunc);




/*
**	This function draws the sample tree structure.
*/
STATIC VOID DrawSampleTree( Object *ltobj )
{
	struct MUI_NListtree_TreeNode *tn1, *tn2, *tn3;
	char txt[128];
	WORD i = 0, j;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tntest = tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn3, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	for( j = 0; j < 26; j++ )
	{
		DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	}

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	for( j = 0; j < 11; j++ )
	{
		tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	}

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;


	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, (APTR)sa[i].flags, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	for( i = 0; i < 2500; i++ )
	{
		if ( i % 2 )	sprintf( txt, "Sort Entry %ld", i + 1 );
		else			sprintf( txt, "Entry Sort %ld", i + 1 );

		tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, txt, 0, ( i % 5 ) ? tn2 : tn1, MUIV_NListtree_Insert_PrevNode_Tail, ( i % 5 ) ? TNF_LIST : 0 );
	}


	DoMethod( ltobj, MUIM_NListtree_InsertStruct, "This/is/a/very/long/test/for/Blafasel_InsertStruct", 0, "/", 0 );
	DoMethod( ltobj, MUIM_NListtree_InsertStruct, "This/is/another/very/long/test/for/Blafasel_InsertStruct", 0, "/", 0 );
}



__inline struct MUI_NListtree_TreeNode *GetParent( Object *obj, struct MUI_NListtree_TreeNode *tn )
{
	if ( tn != NULL )
	{
		return( (struct MUI_NListtree_TreeNode *)DoMethod( obj, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, MUIV_NListtree_GetEntry_Position_Parent, 0 ) );
	}

	return( NULL );
}


__inline struct MUI_NListtree_TreeNode *GetParentNotRoot( Object *obj, struct MUI_NListtree_TreeNode *tn )
{
	if ( tn = GetParent( obj, tn ) )
	{
		if ( GetParent( obj, tn ) )
		{
			return( tn );
		}
	}

	return( NULL );
}


struct MUI_NListtree_TreeNode *IsXChildOfY( Object *obj, struct MUI_NListtree_TreeNode *x, struct MUI_NListtree_TreeNode *y )
{
	do
	{
		if ( y == x )
		{
			return( y );
		}
	}
	while( y = GetParentNotRoot( obj, y ) );

	return( NULL );
}



/*
**	Allocates memory for each entry we create.
*/
LONG ASM SAVEDS confunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_ConstructMessage *msg GNUCREG(a1) )
{
	struct SampleArray *sa;

	/*
	**	Allocate needed piece of memory for the new entry.
	*/
	if ( sa = (struct SampleArray *)AllocVec( sizeof( struct SampleArray) + strlen( msg->Name ) + 1, MEMF_CLEAR ) )
	{
		/*
		**	Save the user data field right after the
		**	array structure.
		*/
		strcpy( (STRPTR)&sa[1], msg->Name );
		sa->name = (STRPTR)&sa[1];

		sa->flags = (UWORD)msg->UserData;
	}

	return( (LONG)sa );
}



/*
**	Free memory we just allocated above.
*/
LONG ASM SAVEDS desfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_DestructMessage *msg GNUCREG(a1) )
{
	if ( msg->UserData != NULL )
	{
		FreeVec( msg->UserData );
		msg->UserData = NULL;
	}

	return( 0 );
}



/*
**	Compare hook function.
*/
LONG ASM SAVEDS compfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_CompareMessage *msg GNUCREG(a1) )
{
	return( stricmp( msg->TreeNode1->tn_Name, msg->TreeNode2->tn_Name ) );
}


/*
**	MultiTest hook function.
*/
LONG ASM SAVEDS mtfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_MultiTestMessage *msg GNUCREG(a1) )
{
	if ( msg->TreeNode->tn_Flags & TNF_LIST )
		return( FALSE );

	return( TRUE );
}



/*
**	Format the entry data for displaying.
*/
LONG ASM SAVEDS dspfunc( REG(a0) struct Hook *hook GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_DisplayMessage *msg GNUCREG(a1) )
{
	STATIC STRPTR t1 = "Newsgroups", t2 = "Flags", t3 = "subscribed", t4 = "\0", t5 = "Count";
	STATIC char buf[10];

	if ( msg->TreeNode != NULL )
	{
		/*
		**	The user data is a pointer to a SampleArray struct.
		*/
		struct SampleArray *a = (struct SampleArray *)msg->TreeNode->tn_User;

		sprintf( buf, "%3ld", msg->Array[-1] );

		*msg->Array++	= msg->TreeNode->tn_Name;
		*msg->Array++	= ( a->flags & 0x8000 ) ? t3 : t4;
		*msg->Array++	= buf;
	}
	else
	{
		*msg->Array++	= t1;
		*msg->Array++	= t2;
		*msg->Array++	= t5;

		*msg->Preparse++	= "\033b\033u";
		*msg->Preparse++	= "\033b\033u";
		*msg->Preparse++	= "\033b\033u";
	}

	return( 0 );
}



/*
**	Insert a new entry which name is given in
**	the string gadget.
*/
LONG SAVEDS ASM insertfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	STRPTR x;

	/*
	**	Get user string.
	*/
	get( st_string, MUIA_String_Contents, &x );

	/*
	**	Insert the new entry after
	**	the active entry.
	*/
	DoMethod( obj, MUIM_NListtree_Insert, x, 0, MUIV_NListtree_Insert_ListNode_Active,
		MUIV_NListtree_Insert_PrevNode_Active, MUIV_NListtree_Insert_Flag_Active );

	return( 0 );
}



/*
**	Exchange two entries.
*/
LONG SAVEDS ASM exchangefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) )
{
	STATIC struct MUI_NListtree_TreeNode *tn1, *tn2;
	STATIC LONG exchcnt = 0;
	LONG x;

	if ( ( exchcnt == 0 ) && ( (ULONG)*para == 42 ) )
	{
		get( obj, MUIA_NListtree_Active, &tn1 );

		if ( tn1 != MUIV_NListtree_Active_Off )
		{
			nnset( tx_info3, MUIA_Text_Contents, "Select entry to exchange selected entry with." );

			exchcnt++;
		}
	}

	else if ( exchcnt == 1 )
	{
		get( obj, MUIA_NListtree_Active, &tn2 );

		if ( ( tn2 != MUIV_NListtree_Active_Off ) && ( tn1 != tn2 ) )
		{
			if ( !IsXChildOfY( obj, tn1, tn2 ) && !IsXChildOfY( obj, tn1, tn2 ) )
			{
				struct MUI_NListtree_ListNode *ln1;

				if ( ln1 = (struct MUI_NListtree_ListNode *)DoMethod( obj, MUIM_NListtree_GetEntry, tn1, MUIV_NListtree_GetEntry_Position_Parent, 0 ) )
				{
					DoMethod( obj, MUIM_NListtree_Exchange, ln1, tn1, MUIV_NListtree_Exchange_ListNode2_Active, MUIV_NListtree_Exchange_TreeNode2_Active, 0 );

					nnset( tx_info3, MUIA_Text_Contents, "Entries successfully exchanged!" );
				}
				else
					nnset( tx_info3, MUIA_Text_Contents, "Something went wrong! Try again to select." );
			}
			else
				nnset( tx_info3, MUIA_Text_Contents, "You can not exchange with childs!" );
		}
		else
			nnset( tx_info3, MUIA_Text_Contents, "You should not exchange an entry with itself!" );

		exchcnt = 0;
	}

	return( 0 );
}



/*
**	Rename the selected entry with the name is given in
**	the string gadget.
*/
LONG SAVEDS ASM renamefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	struct MUI_NListtree_TreeNode *tn;
	STRPTR x;

	/*
	**	Get user string.
	*/
	get( st_string, MUIA_String_Contents, &x );
	get( obj, MUIA_NListtree_Active, &tn );

	/*
	**	Insert the new entry sorted (compare hook)
	**	into the active list node.
	*/
	DoMethod( obj, MUIM_NListtree_Rename, tn,
		x, 0 );

	return( 0 );
}



/*
**	Insert a new entry which name is given in
**	the string gadget.
*/
LONG SAVEDS ASM movefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) )
{
	STATIC struct MUI_NListtree_TreeNode *tn1, *tn2;
	STATIC LONG movecnt = 0;
	LONG x;

	if ( ( movecnt == 0 ) && ( (ULONG)*para == 42 ) )
	{
		get( obj, MUIA_NListtree_Active, &tn1 );

		if ( tn1 != MUIV_NListtree_Active_Off )
		{
			nnset( tx_info3, MUIA_Text_Contents, "Select entry to insert after by simple click." );

			movecnt++;
		}
	}

	else if ( movecnt == 1 )
	{
		get( obj, MUIA_NListtree_Active, &tn2 );

		if ( ( tn2 != MUIV_NListtree_Active_Off ) && ( tn1 != tn2 ) )
		{
			if ( !IsXChildOfY( obj, tn1, tn2 ) && !IsXChildOfY( obj, tn1, tn2 ) )
			{
				struct MUI_NListtree_ListNode *ln1;

				if ( ln1 = (struct MUI_NListtree_ListNode *)DoMethod( obj, MUIM_NListtree_GetEntry, tn1, MUIV_NListtree_GetEntry_Position_Parent, 0 ) )
				{
					DoMethod( obj, MUIM_NListtree_Move, ln1, tn1, MUIV_NListtree_Move_NewListNode_Active, tn2, 0 );

					nnset( tx_info3, MUIA_Text_Contents, "Entry successfully moved!" );
				}
				else
					nnset( tx_info3, MUIA_Text_Contents, "Something went wrong! Try again to select destination." );
			}
			else
				nnset( tx_info3, MUIA_Text_Contents, "You can not move childs!" );
		}
		else
			nnset( tx_info3, MUIA_Text_Contents, "You should not move an entry to itself!" );

		movecnt = 0;
	}

	return( 0 );
}


/*
**	Insert a new entry which name is given in
**	the string gadget.
*/
LONG SAVEDS ASM copyfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) )
{
	STATIC struct MUI_NListtree_TreeNode *tn1, *tn2;
	STATIC LONG copycnt = 0;
	LONG x;

	if ( ( copycnt == 0 ) && ( (ULONG)*para == 42 ) )
	{
		get( obj, MUIA_NListtree_Active, &tn1 );

		if ( tn1 != MUIV_NListtree_Active_Off )
		{
			nnset( tx_info3, MUIA_Text_Contents, "Select entry to insert after by simple click." );

			copycnt++;
		}
	}

	else if ( copycnt == 1 )
	{
		get( obj, MUIA_NListtree_Active, &tn2 );

		if ( ( tn2 != MUIV_NListtree_Active_Off ) && ( tn1 != tn2 ) )
		{
			struct MUI_NListtree_ListNode *ln1;

			if ( ln1 = (struct MUI_NListtree_ListNode *)DoMethod( obj, MUIM_NListtree_GetEntry, tn1, MUIV_NListtree_GetEntry_Position_Parent, 0 ) )
			{
				DoMethod( obj, MUIM_NListtree_Copy, ln1, tn1, MUIV_NListtree_Copy_DestListNode_Active, tn2, 0 );

				nnset( tx_info3, MUIA_Text_Contents, "Entry successfully copied!" );
			}
			else
				nnset( tx_info3, MUIA_Text_Contents, "Something went wrong! Try again to select destination." );
		}
		else
			nnset( tx_info3, MUIA_Text_Contents, "You should not copy an entry to itself!" );

		copycnt = 0;
	}

	return( 0 );
}



/*
**	Move KeepStructure
*/
LONG SAVEDS ASM moveksfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) )
{
	STATIC struct MUI_NListtree_TreeNode *tn1, *tn2;
	STATIC LONG movekscnt = 0;
	LONG x;

	if ( ( movekscnt == 0 ) && ( (ULONG)*para == 42 ) )
	{
		get( obj, MUIA_NListtree_Active, &tn1 );

		if ( tn1 != MUIV_NListtree_Active_Off )
		{
			nnset( tx_info3, MUIA_Text_Contents, "Select entry to make KeepStructure move with." );

			movekscnt++;
		}
	}

	else if ( movekscnt == 1 )
	{
		get( obj, MUIA_NListtree_Active, &tn2 );

		if ( ( tn2 != MUIV_NListtree_Active_Off ) && ( tn1 != tn2 ) )
		{
			struct MUI_NListtree_ListNode *ln1;

			if ( ln1 = (struct MUI_NListtree_ListNode *)DoMethod( obj, MUIM_NListtree_GetEntry, tn1, MUIV_NListtree_GetEntry_Position_Parent, 0 ) )
			{
				DoMethod( obj, MUIM_NListtree_Move, ln1, tn1, MUIV_NListtree_Move_NewListNode_Active, tn2, MUIV_NListtree_Move_Flag_KeepStructure );

				nnset( tx_info3, MUIA_Text_Contents, "Entry successfully moved (structure keeped)" );
			}
			else
				nnset( tx_info3, MUIA_Text_Contents, "Something went wrong! Try again to select destination." );
		}
		else
			nnset( tx_info3, MUIA_Text_Contents, "You should not move an entry to itself!" );

		movekscnt = 0;
	}

	return( 0 );
}


/*
**	Copy KeepStructure
*/
LONG SAVEDS ASM copyksfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) ULONG **para GNUCREG(a1) )
{
	STATIC struct MUI_NListtree_TreeNode *tn1, *tn2;
	STATIC LONG copykscnt = 0;
	LONG x;

	if ( ( copykscnt == 0 ) && ( (ULONG)*para == 42 ) )
	{
		get( obj, MUIA_NListtree_Active, &tn1 );

		if ( tn1 != MUIV_NListtree_Active_Off )
		{
			nnset( tx_info3, MUIA_Text_Contents, "Select entry to make KeepStructure copy with." );

			copykscnt++;
		}
	}

	else if ( copykscnt == 1 )
	{
		get( obj, MUIA_NListtree_Active, &tn2 );

		if ( ( tn2 != MUIV_NListtree_Active_Off ) && ( tn1 != tn2 ) )
		{
			struct MUI_NListtree_ListNode *ln1;

			if ( ln1 = (struct MUI_NListtree_ListNode *)DoMethod( obj, MUIM_NListtree_GetEntry, tn1, MUIV_NListtree_GetEntry_Position_Parent, 0 ) )
			{
				DoMethod( obj, MUIM_NListtree_Copy, ln1, tn1, MUIV_NListtree_Copy_DestListNode_Active, tn2, MUIV_NListtree_Copy_Flag_KeepStructure );

				nnset( tx_info3, MUIA_Text_Contents, "Entry successfully copied (structure keeped)" );
			}
			else
				nnset( tx_info3, MUIA_Text_Contents, "Something went wrong! Try again to select destination." );
		}
		else
			nnset( tx_info3, MUIA_Text_Contents, "You should not copy an entry to itself!" );

		copykscnt = 0;
	}

	return( 0 );
}



LONG ASM SAVEDS fudf( REG(a0) struct Hook *h GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) struct MUIP_NListtree_FindUserDataMessage *msg GNUCREG(a1) )
{
	nnset( tx_info1, MUIA_Text_Contents, "FindUserData Hook passed!" );
	return( strncmp( (STRPTR)msg->User, (STRPTR)msg->UserData, strlen( (STRPTR)msg->User ) ) );
}

#ifdef __AROS__
STATIC struct Hook fudh = { {NULL, NULL}, HookEntry, (void *)fudf, NULL };
#else
STATIC struct Hook fudh = { {NULL, NULL}, (void *)fudf, NULL, NULL };
#endif

/*
**	Find the specified tree node by name.
*/
LONG SAVEDS ASM findnamefunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	struct MUI_NListtree_TreeNode *tn;
	char buf[100];
	STRPTR x;

	/*
	**	Let us see, which string the user wants to search for...
	*/
	get( st_string, MUIA_String_Contents, &x );

	/*
	**	Is it somewhere in the tree?
	*/
	if ( tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, x, MUIV_NListtree_FindUserData_Flag_Activate ) )
	{
		/*
		**	Found! Inform the user.
		*/
		nnset( tx_info3, MUIA_Text_Contents, "Found your node!" );
	}
	else
	{
		/*
		**	Not found. Inform the user.
		*/
		nnset( tx_info3, MUIA_Text_Contents, "NOT found specified node!" );
	}

	return( 0 );
}



/*
**	Sort the active list.
*/
LONG SAVEDS ASM sortfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	struct MUI_NListtree_TreeNode *tn;
	char buf[100];
	clock_t start, end;
	LONG lastactive;

	get( obj, MUIA_NListtree_Active, &lastactive );
	set( obj, MUIA_NListtree_Active, MUIV_NListtree_Active_Off );

	start = clock();
	DoMethod( obj, MUIM_NListtree_Sort, lastactive, 0 );
	end = clock();

	set( obj, MUIA_NListtree_Active, lastactive );

	DoMethod( tx_info3, MUIM_SetAsString, MUIA_Text_Contents, "Sort took %ld.%03lds", ( end - start ) / CLOCKS_PER_SEC, ( end - start ) % CLOCKS_PER_SEC );

	return( 0 );
}



/*
**	Find the specified tree node by name.
*/
LONG SAVEDS ASM getnrfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	LONG temp, temp2;

	temp = DoMethod( obj, MUIM_NListtree_GetNr,
		MUIV_NListtree_GetNr_TreeNode_Active, MUIV_NListtree_GetNr_Flag_CountLevel );

	if ( temp == 1 )
		DoMethod( tx_info1, MUIM_SetAsString, MUIA_Text_Contents, "1 entry in parent node." );
	else
		DoMethod( tx_info1, MUIM_SetAsString, MUIA_Text_Contents, "%ld entries in parent node.", temp );


	temp = DoMethod( obj, MUIM_NListtree_GetNr,
		MUIV_NListtree_GetNr_TreeNode_Active, MUIV_NListtree_GetNr_Flag_CountAll );

	if ( temp == 1 )
		DoMethod( tx_info2, MUIM_SetAsString, MUIA_Text_Contents, "1 entry total." );
	else
		DoMethod( tx_info2, MUIM_SetAsString, MUIA_Text_Contents, "%ld entries total.", temp );


	temp = DoMethod( obj, MUIM_NListtree_GetNr,
		MUIV_NListtree_GetNr_TreeNode_Active, 0 );

	temp2 = DoMethod( obj, MUIM_NListtree_GetNr,
		MUIV_NListtree_GetNr_TreeNode_Active, MUIV_NListtree_GetNr_Flag_Visible );

	DoMethod( tx_info3, MUIM_SetAsString, MUIA_Text_Contents, "Active entry pos: %ld (visible: %ld).", temp, temp2 );

	return( 0 );
}


/*
**	Find the specified tree node by name.
*/
LONG SAVEDS ASM numselfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	LONG temp = 0;

	DoMethod( obj, MUIM_NListtree_Select, MUIV_NListtree_Select_All,
		MUIV_NListtree_Select_Ask, 0, &temp );

	if ( temp == 1 )
		DoMethod( tx_info1, MUIM_SetAsString, MUIA_Text_Contents, "1 node selected." );
	else
		DoMethod( tx_info1, MUIM_SetAsString, MUIA_Text_Contents, "%ld nodes selected.", temp );

	{
		struct MUI_NListtree_TreeNode *tn;

		tn = (struct MUI_NListtree_TreeNode *)MUIV_NListtree_NextSelected_Start;

		for (;;)
		{
			DoMethod( obj, MUIM_NListtree_NextSelected, &tn );

			if ( (LONG)tn == MUIV_NListtree_NextSelected_End )
				break;

			D(bug( "Next TreeNode: 0x%08lx - %s\n", tn, tn->tn_Name ) );
		}

		D(bug( "\n" ) );

		tn = (struct MUI_NListtree_TreeNode *)MUIV_NListtree_PrevSelected_Start;

		for (;;)
		{
			DoMethod( obj, MUIM_NListtree_PrevSelected, &tn );

			if ( (LONG)tn == MUIV_NListtree_PrevSelected_End )
				break;

			D(bug( "Prev TreeNode: 0x%08lx - %s\n", tn, tn->tn_Name ) );
		}
	}

	return( 0 );
}


/*
**	Test func
*/
LONG SAVEDS ASM testfunc(REG(a0) struct Hook *unused_hook GNUCREG(a0), REG(a2) APTR obj GNUCREG(a2), REG(a1) APTR unused_msg GNUCREG(a1) )
{
	ULONG id;
	ULONG num;

	id = MUIV_NListtree_NextSelected_Start;

	for(;;)
	{
		DoMethod( obj, MUIM_NListtree_NextSelected, &id );

		if( id == MUIV_NListtree_NextSelected_End )
			break;

		//GetAttr( MUIA_List_Entries, obj, &num );

		num = DoMethod( obj, MUIM_NListtree_GetNr,
				MUIV_NListtree_GetNr_TreeNode_Active, MUIV_NListtree_GetNr_Flag_CountAll );

		if ( num > 1 )
			DoMethod( obj, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Active, id, 0 );
		else
			break;
	}

	return( 0 );
}





/*
**	Main
*/
int main( int argc, STRPTR argv[] )
{
	ULONG signals;

	MUIMasterBase = OpenLibrary( "muimaster.library", 19 );
	IntuitionBase = (struct IntuitionBase *)OpenLibrary( "intuition.library", 36 );

	/*
	**	Is MUI V19 available?
	*/
	if ( MUIMasterBase && IntuitionBase )
	{
		/*
		**	Create application object.
		*/
		app = ApplicationObject,
			MUIA_Application_Title,			"NListtree-Demo",
			MUIA_Application_Copyright,		"©1999-2000 by Apocalypse Hard- and Software",
			MUIA_Application_Author,		"Carsten Scholling",
			MUIA_Application_Description,	"Demonstration program for MUI class NListtree.mcc",
			MUIA_Application_Base,			"NLISTTREEDEMO",

			/*
			**	Build the window.
			*/
			SubWindow, window = WindowObject,
				MUIA_Window_Title,			"NListtree-Demo",
				MUIA_Window_ID,				MAKE_ID( 'N', 'L', 'T', 'R' ),
				MUIA_Window_AppWindow,		TRUE,
				WindowContents,				VGroup,

					/*
					**	Create a NListview embedded NListtree object
					*/
					Child, NListviewObject,
						MUIA_ShortHelp,			"The NListtree object...",
						MUIA_NListview_NList,	lt_nodes = NListtreeObject,
							InputListFrame,
							MUIA_CycleChain,				TRUE,
							MUIA_NList_MinLineHeight,		18,
							MUIA_NListtree_MultiSelect,		MUIV_NListtree_MultiSelect_Shifted,
							//MUIA_NListtree_MultiTestHook,	&mthook,
							MUIA_NListtree_DisplayHook,		&dsphook,
							MUIA_NListtree_ConstructHook,	&conhook,
							MUIA_NListtree_DestructHook,	&deshook,	/* This is the same as MUIV_NListtree_CompareHook_LeavesMixed. */
							MUIA_NListtree_CompareHook,		&comphook,
							MUIA_NListtree_DoubleClick,		MUIV_NListtree_DoubleClick_Tree,
							MUIA_NListtree_EmptyNodes,		FALSE,
							MUIA_NListtree_TreeColumn,		0,
							MUIA_NListtree_DragDropSort,	TRUE,
							MUIA_NListtree_Title,			TRUE,
							MUIA_NListtree_Format,			",,",
							MUIA_NListtree_FindUserDataHook,&fudh,
							//MUIA_NListtree_NoRootTree,		TRUE,
						End,
					End,

					/*
					**	Build some controls.
					*/
					Child, tx_info1 = TextObject,
						MUIA_Background, MUII_TextBack,
						TextFrame,
					End,

					Child, tx_info2 = TextObject,
						MUIA_Background, MUII_TextBack,
						TextFrame,
					End,

					Child, tx_info3 = TextObject,
						MUIA_Background, MUII_TextBack,
						TextFrame,
					End,

					Child, ColGroup( 2 ),
						Child, FreeKeyLabel( "TreeCol:", 'c' ),
						Child, sl_treecol	= Slider( 0, 2, 0 ),
					End,

					Child, HGroup,
						Child, st_string = StringObject,
							StringFrame,
							MUIA_String_MaxLen, 50,
						End,
					End,


					Child, ColGroup( 4 ),
						Child, bt_open		= KeyButton( "Open",		'o' ),
						Child, bt_close		= KeyButton( "Close",		'c' ),
						Child, bt_expand	= KeyButton( "Expand",		'e' ),
						Child, bt_collapse	= KeyButton( "Collapse",	'a' ),

						Child, bt_insert	= KeyButton( "Insert",		'i' ),
						Child, bt_remove	= KeyButton( "Remove",		'r' ),
						Child, bt_exchange	= KeyButton( "Exchange",	'x' ),
						Child, bt_rename	= KeyButton( "Rename",		'r' ),

						Child, bt_move		= KeyButton( "Move",		'm' ),
						Child, bt_copy		= KeyButton( "Copy",		'y' ),
						Child, bt_moveks	= KeyButton( "Move KS",		'v' ),
						Child, bt_copyks	= KeyButton( "Copy KS",		'k' ),

						Child, bt_find		= KeyButton( "FindName",	'f' ),
						Child, bt_parent	= KeyButton( "Parent",		'p' ),
						Child, bt_sort		= KeyButton( "Sort",		's' ),
						Child, bt_getnr		= KeyButton( "GetNr",		'n' ),

						Child, bt_redraw	= KeyButton( "Redraw",		'w' ),
						Child, bt_selected	= KeyButton( "Selected",	'd' ),
						Child, bt_seltogg	= KeyButton( "Sel Togg",	't' ),
						Child, bt_showtree	= KeyButton( "Show tree",	'h' ),

						Child, bt_test		= KeyButton( "Test", ' ' ),
						Child, bt_test2		= KeyButton( "Test2", ' ' ),
						Child, bt_test3		= KeyButton( "Test3", ' ' ),
						Child, bt_test4		= KeyButton( "Test4", ' ' ),
					End,

				End,

			End,
		End;


		if( app )
		{
			/*
			**	generate notifications
			*/
			DoMethod( window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

			/*
			**	open/close/expand/collapse
			*/
			DoMethod( bt_open, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Active, MUIV_NListtree_Open_TreeNode_Active, 0 );

			DoMethod( bt_close, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Close, MUIV_NListtree_Close_ListNode_Active, MUIV_NListtree_Close_TreeNode_Active, 0 );

			DoMethod( bt_expand, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Root, MUIV_NListtree_Open_TreeNode_All, 0 );

			DoMethod( bt_collapse, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Close, MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0 );



			/*
			**	insert/remove/exchange/rename
			*/
			DoMethod( bt_insert, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &inserthook );

			DoMethod( bt_remove, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, MUIV_NListtree_Remove_TreeNode_Selected, 0 );

			DoMethod( bt_exchange, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_CallHook, &exchangehook, 42 );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				lt_nodes, 3, MUIM_CallHook, &exchangehook, 0 );

			DoMethod( bt_rename, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &renamehook );


			/*
			**	move/copy/moveks/copyks
			*/
			DoMethod( bt_move, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_CallHook, &movehook, 42 );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				lt_nodes, 3, MUIM_CallHook, &movehook, 0 );

			DoMethod( bt_copy, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_CallHook, &copyhook, 42 );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				lt_nodes, 3, MUIM_CallHook, &copyhook, 0 );

			DoMethod( bt_moveks, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_CallHook, &movekshook, 42 );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				lt_nodes, 3, MUIM_CallHook, &movekshook, 0 );

			DoMethod( bt_copyks, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_CallHook, &copykshook, 42 );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				lt_nodes, 3, MUIM_CallHook, &copykshook, 0 );


			/*
			**	find/parent/sort/getnr
			*/
			DoMethod( bt_find, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &findnamehook );

			DoMethod( bt_parent, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_Set, MUIA_NListtree_Active, MUIV_NListtree_Active_Parent );

			DoMethod( bt_sort, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &sorthook );

			/*
			DoMethod( bt_sort, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_NListtree_Sort, MUIV_NListtree_Sort_TreeNode_Active, 0 );
			*/

			DoMethod( bt_getnr, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &getnrhook );


			/*
			**	redraw/selected/seltogg/showtree
			*/
			DoMethod( bt_redraw, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_NListtree_Redraw, MUIV_NListtree_Redraw_All );

			DoMethod( bt_selected, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &numselhook );

			DoMethod( bt_seltogg, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 4, MUIM_NListtree_Select, MUIV_NListtree_Select_All, MUIV_NListtree_Select_Toggle, 0); /* stegerg: FIXME, State param omitted! Crash! */

			DoMethod( bt_showtree, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 3, MUIM_Set, MUIA_NListtree_ShowTree, MUIV_NListtree_ShowTree_Toggle );


			/*
			**	misc
			*/
			DoMethod( sl_treecol, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				lt_nodes, 3, MUIM_Set, MUIA_NListtree_TreeColumn, MUIV_TriggerValue );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
				tx_info1, 4, MUIM_SetAsString, MUIA_Text_Contents, "Active node: 0x%08lx", MUIV_TriggerValue );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_ActiveList, MUIV_EveryTime,
				tx_info2, 4, MUIM_SetAsString, MUIA_Text_Contents, "Active list: 0x%08lx", MUIV_TriggerValue );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime,
				tx_info3, 4, MUIM_SetAsString, MUIA_Text_Contents, "Double clicked on node: 0x%08lx", MUIV_TriggerValue );

			DoMethod( lt_nodes, MUIM_Notify, MUIA_NListtree_SelectChange, TRUE,
				tx_info3, 3, MUIM_SetAsString, MUIA_Text_Contents, "Selection state changed" );


			/*
			**	test
			*/
			DoMethod( bt_test, MUIM_Notify, MUIA_Pressed, FALSE,
				lt_nodes, 2, MUIM_CallHook, &testhook );



			/*
			**	Open the window.
			**
			*/
			set( window, MUIA_Window_Open, TRUE );


			/*
			**	Set the tree into quiet state.
			*/
			set( lt_nodes, MUIA_NListtree_Quiet, TRUE );


			/*
			**	Insert sample nodes.
			*/
			DrawSampleTree( lt_nodes );


			/*
			**	Set the tree back to normal state.
			*/
			set( lt_nodes, MUIA_NListtree_Quiet, FALSE );

			/*
			**	Minimal input loop.
			*/
			while( DoMethod( app, MUIM_Application_NewInput, &signals ) != MUIV_Application_ReturnID_Quit )
			{
				if ( signals )
				{
					signals = Wait( signals | SIGBREAKF_CTRL_C );

					if ( signals & SIGBREAKF_CTRL_C )
						break;
				}
			}

			/*
			**	Clear the list.
			*/
			DoMethod( lt_nodes, MUIM_NListtree_Clear, NULL, 0 );


			/*
			**	Close the window.
			*/
			set( window, MUIA_Window_Open, FALSE );


			/*
			**	Shutdown
			*/
			MUI_DisposeObject( app );
		}
		else
			printf( "Failed to create Application.\n" );
	}

	if ( IntuitionBase )
		CloseLibrary( (struct Library *)IntuitionBase );

	if ( MUIMasterBase )
		CloseLibrary( MUIMasterBase );

	return( 0 );
}


