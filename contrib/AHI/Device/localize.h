/* $Id$
* $Log$
* Revision 1.5  2004/08/04 17:45:05  stegerg
* Updated sources to AHI 5.13
*
* Revision 5.3.2.1  2004/06/08 21:16:12  martin
* Added 7.1 multichannel support.
*
* Revision 5.0  2000/11/28 00:15:18  lcs
* Bumped CVS revision to 5.0.
*
* Revision 4.1  1997/04/02 22:29:53  lcs
* Bumped to version 4
*
* Revision 1.1  1997/02/03 16:22:45  lcs
* Initial revision
*
*/
/****************************************************************
   This file was created automatically by `FlexCat 2.4'
   from "../../ahisrc/Device/ahi.cd".

   Do NOT edit by hand!
****************************************************************/

#ifndef ahi_CAT_H
#define ahi_CAT_H


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif	/*  !EXEC_TYPES_H	    */
#ifndef LIBRARIES_LOCALE_H
#include <libraries/locale.h>
#endif	/*  !LIBRARIES_LOCALE_H     */


/*  Prototypes	*/
extern void OpenahiCatalog(struct Locale *, STRPTR);
extern void CloseahiCatalog(void);
extern STRPTR GetahiString(APTR);

extern struct Catalog * ExtOpenCatalog(struct Locale *, STRPTR);
extern void ExtCloseCatalog(struct Catalog *);
extern STRPTR GetString(APTR, struct Catalog *);


/*  Definitions */
extern const APTR _msgDefault;
#define msgDefault ((APTR) &_msgDefault)
extern const APTR _msgMenuControl;
#define msgMenuControl ((APTR) &_msgMenuControl)
extern const APTR _msgMenuLastMode;
#define msgMenuLastMode ((APTR) &_msgMenuLastMode)
extern const APTR _msgMenuNextMode;
#define msgMenuNextMode ((APTR) &_msgMenuNextMode)
extern const APTR _msgMenuPropertyList;
#define msgMenuPropertyList ((APTR) &_msgMenuPropertyList)
extern const APTR _msgMenuRestore;
#define msgMenuRestore ((APTR) &_msgMenuRestore)
extern const APTR _msgMenuOK;
#define msgMenuOK ((APTR) &_msgMenuOK)
extern const APTR _msgMenuCancel;
#define msgMenuCancel ((APTR) &_msgMenuCancel)
extern const APTR _msgUnknown;
#define msgUnknown ((APTR) &_msgUnknown)
extern const APTR _msgReqOK;
#define msgReqOK ((APTR) &_msgReqOK)
extern const APTR _msgReqCancel;
#define msgReqCancel ((APTR) &_msgReqCancel)
extern const APTR _msgReqFrequency;
#define msgReqFrequency ((APTR) &_msgReqFrequency)
extern const APTR _msgDefaultMode;
#define msgDefaultMode ((APTR) &_msgDefaultMode)
extern const APTR _msgReqInfoTitle;
#define msgReqInfoTitle ((APTR) &_msgReqInfoTitle)
extern const APTR _msgReqInfoAudioID;
#define msgReqInfoAudioID ((APTR) &_msgReqInfoAudioID)
extern const APTR _msgReqInfoResolution;
#define msgReqInfoResolution ((APTR) &_msgReqInfoResolution)
extern const APTR _msgReqInfoMono;
#define msgReqInfoMono ((APTR) &_msgReqInfoMono)
extern const APTR _msgReqInfoStereo;
#define msgReqInfoStereo ((APTR) &_msgReqInfoStereo)
extern const APTR _msgReqInfoStereoPan;
#define msgReqInfoStereoPan ((APTR) &_msgReqInfoStereoPan)
extern const APTR _msgReqInfoChannels;
#define msgReqInfoChannels ((APTR) &_msgReqInfoChannels)
extern const APTR _msgReqInfoMixrate;
#define msgReqInfoMixrate ((APTR) &_msgReqInfoMixrate)
extern const APTR _msgReqInfoHiFi;
#define msgReqInfoHiFi ((APTR) &_msgReqInfoHiFi)
extern const APTR _msgReqInfoRecordHalf;
#define msgReqInfoRecordHalf ((APTR) &_msgReqInfoRecordHalf)
extern const APTR _msgReqInfoRecordFull;
#define msgReqInfoRecordFull ((APTR) &_msgReqInfoRecordFull)
extern const APTR _msgReqInfoMultiChannel;
#define msgReqInfoMultiChannel ((APTR) &_msgReqInfoMultiChannel)

#endif /*   !ahi_CAT_H  */
