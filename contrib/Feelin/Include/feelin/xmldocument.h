#ifndef FEELIN_XMLDOCUMENT_H
#define FEELIN_XMLDOCUMENT_H

#include <feelin/document.h>

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */
 
/*
**  $VER: feelin/xmldocument.h 2.0 (2005/05/01)
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#define FC_XMLDocument                          "XMLDocument"
#define XMLDocumentObject                       F_NewObj(FC_XMLDocument

enum    {

		FV_XMLDOCUMENT_ID_VERSION = FV_DOCUMENT_ID_DUMMY,

		FV_XMLDOCUMENT_ID_DUMMY = FV_DOCUMENT_ID_DUMMY + 32

		};

/*** types *****************************************************************/

typedef struct FeelinXMLAttribute
{
	struct FeelinXMLAttribute      *Next;
	struct FeelinXMLAttribute      *Prev;

	FDOCName                       *Name;
	STRPTR                          Data;
	
	ULONG                           Line;
}
FXMLAttribute;

typedef struct FeelinXMLMarkup
{
	struct FeelinXMLMarkup         *Next;
	struct FeelinXMLMarkup         *Prev;

	FDOCName                       *Name;
	STRPTR                          Body;

	ULONG                           Line;

	FList                           AttributesList;
	FList                           ChildrenList;
}
FXMLMarkup;

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
