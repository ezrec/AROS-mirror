#ifndef FEELIN_XMLOBJECT_H
#define FEELIN_XMLOBJECT_H

#include <feelin/xmldocument.h>

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */

/*
**  $VER: feelin/xmlobject.h 1.0 (2005/04/30)
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#define FC_XMLObject                            "XMLObject"
#define XMLObjectObject                         F_NewObj(FC_XMLObject

enum    {

		FV_XMLOBJECT_ID_NAME = FV_XMLDOCUMENT_ID_DUMMY,
		FV_XMLOBJECT_ID_OBJECTS,
		
		FV_XMLOBJECT_ID_DUMMY = FV_XMLDOCUMENT_ID_DUMMY + 32

		};

/*** types *****************************************************************/

typedef struct FeelinXMLReference
{
	STRPTR                          Name;
	FObject                        *ObjectPtr;
}
FXMLReference;

typedef struct FeelinXMLDefinition
{
	STRPTR                          Name;
	APTR                            Data;
}
FXMLDefinition;

/*** messages **************************************************************/

struct  FS_XMLObject_Find                       { STRPTR Name; };
struct  FS_XMLObject_Create                     { FXMLMarkup *Markup; APTR Pool; };

/*** macros ****************************************************************/

#define F_XML_DEFS                              f_auto_xml_defs
#define F_XML_DEFS_INIT(num)                    FXMLDefinition f_auto_xml_defs[num + 1]; ULONG f_auto_xml_defs_count=0
#define F_XML_DEFS_ADD(name,data)               f_auto_xml_defs[f_auto_xml_defs_count].Name = name; f_auto_xml_defs[f_auto_xml_defs_count++].Data = (APTR)(data)
#define F_XML_DEFS_DONE                         f_auto_xml_defs[f_auto_xml_defs_count].Name = NULL

#define F_XML_REFS                              f_auto_xml_refs
#define F_XML_REFS_INIT(num)                    FXMLReference f_auto_xml_refs[num + 1]; ULONG f_auto_xml_refs_count=0
#define F_XML_REFS_ADD(name,ptr)                f_auto_xml_refs[f_auto_xml_refs_count].Name = name; f_auto_xml_refs[f_auto_xml_refs_count++].ObjectPtr = ptr
#define F_XML_REFS_DONE                         f_auto_xml_refs[f_auto_xml_refs_count].Name = NULL

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
