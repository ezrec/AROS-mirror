#ifndef FEELIN_HTMLDOCUMENT_H
#define FEELIN_HTMLDOCUMENT_H

#include <feelin/document.h>

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */
 
/*
**  $VER: feelin/HTMLdocument.h 1.0 (2005/05/02)
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#define FC_HTMLDocument                          "HTMLDocument"
#define HTMLDocumentObject                       F_NewObj(FC_HTMLDocument

enum    {

		FV_HTMLDOCUMENT_ID_VERSION = FV_DOCUMENT_ID_DUMMY,

		FV_HTMLDOCUMENT_ID_DUMMY = FV_DOCUMENT_ID_DUMMY + 32

		};

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

enum    {

		FV_HTMLDocument_Node_Text,
		FV_HTMLDocument_Node_Markup

		};

typedef struct FeelinHTMLNode
{
	struct FeelinHTMLNode          *Next;
	struct FeelinHTMLNode          *Prev;

	UBYTE                           Type;
}
FHTMLNode;
	   
/***************************************************************************/

typedef struct FeelinHTMLText
{
	FHTMLNode                       Node;

	STRPTR                          Data;
	ULONG                           DataLength;
}
FHTMLText;

typedef struct FeelinHTMLMarkup
{
	FHTMLNode                       Node;

	BYTE                            Terminator;
	UWORD                           Line;

	FDOCName                       *Name;

	FList                           AttributesList;
	FList                           ChildrenList;
}
FHTMLMarkup;

typedef struct FeelinHTMLAttribute
{
	struct FeelinHTMLAttribute     *Next;
	struct FeelinHTMLAttribute     *Prev;

	ULONG                           Line;

	FDOCName                       *Name;
	STRPTR                          Data;
}
FHTMLAttribute;

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
