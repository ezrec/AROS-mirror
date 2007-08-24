#ifndef FEELIN_XMLAPPLICATION_H
#define FEELIN_XMLAPPLICATION_H

#include <feelin/xmlobject.h>

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */

/*
**  $VER: feelin/xmlapplication.h 2.0 (2005/03/15)
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#define FC_XMLApplication                       "XMLApplication"
#define XMLApplicationObject                    F_NewObj(FC_XMLApplication

#define FV_XMLApplication_Version               1

enum    {

		FV_XMLAPPLICATION_ID_APPLICATION = FV_XMLOBJECT_ID_DUMMY,
		FV_XMLAPPLICATION_ID_ATTRIBUTE,
		FV_XMLAPPLICATION_ID_MESSAGE,
		FV_XMLAPPLICATION_ID_METHOD,
		FV_XMLAPPLICATION_ID_NOTIFY,
		FV_XMLAPPLICATION_ID_OBJ,
		FV_XMLAPPLICATION_ID_ROOT,
		FV_XMLAPPLICATION_ID_TARGET,
		FV_XMLAPPLICATION_ID_VALUE,
		
		FV_XMLAPPLICATION_ID_DUMMY = FV_XMLOBJECT_ID_DUMMY + 32

		};

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
