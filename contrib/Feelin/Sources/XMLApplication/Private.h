/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <dos/dos.h>
#include <libraries/feelin.h>
#include <feelin/xmldocument.h>
#include <feelin/xmlapplication.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FM_XMLApplication_CreateNotify,
        FM_XMLApplication_Run,
        
        FM_XMLObject_Build

        };

enum    {                                       // resolved

        FM_Document_Resolve,
        FM_Document_Log,
        FM_XMLObject_Find,

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FObject                         Application;
};

#undef F_ERROR
#define F_ERROR(n)                              { er = FV_ERROR_ ## n; goto __error; }

enum  {

      FV_ERROR_DUMMY,
      FV_ERROR_MEMORY,
      FV_ERROR_TYPE,
      FV_ERROR_VERSION,
      FV_ERROR_NULLMarkup

      };

struct FS_XMLApplication_CreateNotify           { FXMLMarkup *Markup; APTR Pool; };

