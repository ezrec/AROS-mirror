#ifndef TEXTEDITFIELD_H
#define TEXTEDITFIELD_H

#define MUIA_TEF_Text	(TAG_USER + 0x0001)		//ISG
#define MUIA_TEF_Height	(TAG_USER + 0x0002)		//I.. /* (ULONG) In pixels */
#define MUIA_TEF_Width	(TAG_USER + 0x0003)		//I.. /* (ULONG) In pixels */
#define MUIA_TEF_Font	(TAG_USER + 0x0004)		//I.. /* (struct TextFont *) */
#define MUIA_TEF_VBar	(TAG_USER + 0x0005)		//I.. /* PRIVATE */
#define MUIA_TEF_HBar	(TAG_USER + 0x0006)		//I.. /* PRIVATE */
#define MUIA_TEF_FirstX	(TAG_USER + 0x0007)		//I..
#define MUIA_TEF_FirstY	(TAG_USER + 0x0008)		//I..
#define MUIA_TEF_CursorState	(TAG_USER + 0x0009)		//IS.
#define MUIA_TEF_Cursor	(TAG_USER + 0x000A)		//I..
#define MUIA_TEF_Active (TAG_USER + 0x000B)		//IS.
#define MUIM_TEF_FileChange (TAG_USER + 0x000C)	// The text has changed on disk.

#define MUIA_TEF_ParentL	(TAG_USER + 0x000D)		//.S. /* PRIVATE */
#define MUIA_TEF_ParentR	(TAG_USER + 0x000E)		//.S. /* PRIVATE */
#define MUIA_TEF_ParentT	(TAG_USER + 0x000F)		//.S. /* PRIVATE */
#define MUIA_TEF_ParentB	(TAG_USER + 0x0010)		//.S. /* PRIVATE */
#define MUIA_TEF_Parent		(TAG_USER + 0x0011)		//I.. /* PRIVATE */
#define MUIA_TEF_ActivateHook (TAG_USER + 0x0012)	//.S. /* struct Hook */

//#define MUIA_TEF_Font	(TAG_USER + 0x0004)		//I..
//#define MUIA_TEF_Font	(TAG_USER + 0x0004)		//I..
//#define MUIA_TEF_Font	(TAG_USER + 0x0004)		//I..


Class *TextEditFieldClInit(void);
BOOL TextEditFieldClFree(Class *cl);

#endif
