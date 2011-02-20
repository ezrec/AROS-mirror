/*
 * @(#) $Header$
 *
 * Change AutoDoc program
 * cad_support.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:10:58  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:23:20  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:55:42  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 02:52:53  mlemos
 * Integrated Anton Rolls changes to generate documentation files in
 * AmigaGuide and HTML formats.
 *
 * Revision 1.1.2.1  1998/09/19 01:50:22  mlemos
 * Ian sources.
 *
 *
 *
 */

#include "cad.h"

/*
 *	Import data.
 */
extern struct Window		*MainWindow;

/*
 *	Show an AmigaDOS error via BGUI_Request().
 */
STATIC ULONG DosRequestA( UBYTE *gadgets, UBYTE *body, IPTR *args)
{
	static struct bguiRequest		req = { (LONG)NULL };
	UBYTE					buffer[ 256 ], ebuf[ 80 ];

	/*
	 *	Copy the body text into the buffer.
	 */
	strcpy( buffer, body );

	/*
	 *	Obtain the textual DOS error.
	 */
	Fault( IoErr(), (LONG)NULL, ebuf, 80 );

	/*
	 *	Add this to the buffer.
	 */
	strncat( buffer, "\n\n\"" ISEQ_HIGHLIGHT, 256 );
	strncat( buffer, ebuf, 256 );
	strncat( buffer, ISEQ_TEXT "\"", 256 );

	/*
	 *	Setup the requester.
	 */
	req.br_Title		=	NAME;
	req.br_TextFormat	=	buffer;
	req.br_GadgetFormat	=	gadgets;
	req.br_Flags		=	BREQF_CENTERWINDOW | BREQF_AUTO_ASPECT | BREQF_LOCKWINDOW;
	req.br_Underscore	=	'_';

	/*
	 *	Pop it.
	 */
	return BGUI_RequestA( MainWindow, &req, args);
}

/*
 *	Show a requester via BGUI_Request().
 */
ULONG MyRequestA( UBYTE *gadgets, UBYTE *body, IPTR *args )
{
	static struct bguiRequest		req = { (LONG)NULL };

	/*
	 *	Setup the requester.
	 */
	req.br_Title		=	NAME;
	req.br_TextFormat	=	body;
	req.br_GadgetFormat	=	gadgets;
	req.br_Flags		=	BREQF_CENTERWINDOW | BREQF_AUTO_ASPECT | BREQF_LOCKWINDOW;
	req.br_Underscore	=	'_';

	/*
	 *	Pop it.
	 */
	return( BGUI_RequestA( MainWindow, &req, args));
}

/*
 *	Set attributes on multiple-objects.
 */
VOID MultiSetAttr( struct GadgetInfo *gi, Tag tag, ULONG data, ... )
{
	Object			*obj;
	struct TagItem		  tags[ 2 ];
	va_list ap;

	va_start(ap, data);

	tags[ 0 ].ti_Tag  = tag;
	tags[ 0 ].ti_Data = data;
	tags[ 1 ].ti_Tag  = TAG_END;

	while ( (obj = va_arg(ap, Object *)) ) {
		DoMethod(obj, OM_SET, &tags[ 0 ], gi );
	}

	va_end(ap);
}

/*
 *	Set attributes to an object.
 */
ULONG SetAttr( Object *obj, struct GadgetInfo *gi, Tag tag1, ... )
{
	AROS_SLOWSTACKTAGS_PRE_AS(tag1, ULONG);
	retval = DoMethod( obj, OM_SET, AROS_SLOWSTACKTAGS_ARG(tag1), gi );
	AROS_SLOWSTACKTAGS_POST
}
