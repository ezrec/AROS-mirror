/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_extras.c
 */

#ifdef __PPC__
#pragma pack(2)
#endif

struct JumpEntry {
    UWORD je_JMPInstruction;
    APTR je_JumpAddress;
};

#ifdef __PPC__
#pragma pack()
#endif

#define WINDOW_TITLE_LENGTH         128

#define SPACE40 "                                        "

BOOL GetPriority( STRPTR , LONG * );

void NoReqOn( void );

void NoReqOff( void );

struct Library *MyOpenLibrary( CONST_STRPTR , ULONG );

void Flags2BinStr( UBYTE , STRPTR , ULONG );

void VARARGS68K STDARGS MySetContents( APTR , CONST_STRPTR , ... );

void VARARGS68K STDARGS MySetContentsHealed( APTR , CONST_STRPTR , ... );

void VARARGS68K STDARGS MySetStringContents(APTR , CONST_STRPTR , ...);

void SetCountText( APTR , ULONG );

STRPTR MyGetWindowTitle( CONST_STRPTR , STRPTR , ULONG );

STRPTR MyGetChildWindowTitle( CONST_STRPTR , CONST_STRPTR, STRPTR, ULONG );

BOOL CheckMCCVersion( CONST_STRPTR mcc, ULONG minver, ULONG minrev, BOOL errorReq );

void ApplicationSleep( BOOL );

void RedrawActiveEntry( APTR );

void RemoveActiveEntry( APTR );

void InsertBottomEntry( APTR , APTR );

void InsertSortedEntry( APTR , APTR );

APTR GetActiveEntry( APTR );

struct MUI_NListtree_TreeNode *GetActiveTreeNode( APTR tree );

APTR MyNListviewObject( APTR * , ULONG , CONST_STRPTR , struct Hook * , struct Hook * , struct Hook * , struct Hook * , BOOL );

APTR MyNListtreeObject( APTR * , CONST_STRPTR , struct Hook * , struct Hook * , struct Hook * , struct Hook * , struct Hook *, ULONG , BOOL);

APTR MyBelowListview( APTR * , APTR * );

#define MyLabel(text)       LabelA(text)
#define MyLabel2(text)      LabelB(text)
#define MyHSpace(space)     HSpace(space)
#define MyVSpace(space)     VSpace(space)

APTR MyTextObject( void );

APTR MyTextObject2( void );

APTR MyTextObject3( CONST_STRPTR );

APTR MyTextObject4( CONST_STRPTR );

APTR MyTextObject5( CONST_STRPTR );

APTR MyTextObject6( void );

APTR MyTextObject7( CONST_STRPTR );

APTR MyCheckmarkImage( void );

BOOL isValidPointer( APTR ptr );
void healstring( STRPTR );
CONST_STRPTR nonetest( CONST_STRPTR );
BOOL points2ram( APTR addr );

STRPTR StripMUIFormatting( STRPTR str );

BOOL IsReal( CONST_STRPTR );
BOOL IsHex( CONST_STRPTR , SIPTR * );
BOOL IsUHex( CONST_STRPTR , IPTR * );
BOOL IsDec( CONST_STRPTR , SIPTR * );

LONG HexCompare( CONST_STRPTR hex1str, CONST_STRPTR hex2str );
LONG IntegerCompare( CONST_STRPTR int1str, CONST_STRPTR int2str );
LONG VersionCompare( CONST_STRPTR ver1str, CONST_STRPTR ver2str );
LONG PriorityCompare( CONST_STRPTR pri1str, CONST_STRPTR pri2str );

void SortLinkedList( struct List *lh, LONG (* compare)( const struct Node *, const struct Node *) );
void FreeLinkedList( struct List *lh );

void GetRamPointerCount( struct Library *lib, LONG *count, LONG *total );

struct List *FindListOfNode( struct Node *ln );

IPTR AllocListEntry( APTR, APTR, ULONG );

void FreeListEntry( APTR, APTR * );

void NameCopy( STRPTR dst, CONST_STRPTR src);

APTR GetApplication( void );

// a little trick, borrowed from YAM
// first we collect all outstanding signals
// then we open the new window, which will perhaps trigger other windows to close, because of "single window" mode
// at last we reissue the formerly collected signals
#define COLLECT_SIZE 32
#define COLLECT_RETURNIDS { \
                            ULONG returnID[COLLECT_SIZE], csize = COLLECT_SIZE, rpos = COLLECT_SIZE, userData, userSigs = 0; \
                            while(csize && userSigs == 0 && (userData = DoMethod(AP_Scout, MUIM_Application_NewInput, &userSigs))) \
                              returnID[--csize] = userData

#define REISSUE_RETURNIDS   while(rpos > csize) \
                              DoMethod(AP_Scout, MUIM_Application_ReturnID, returnID[--rpos]); \
                          }

