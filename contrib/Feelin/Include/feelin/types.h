#ifndef FEELIN_TYPES_H
#define FEELIN_TYPES_H

/*
**  feelin/types.h
**
**  Data typing. Should be included before any other Feelin include.
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 10.00 (2005/08/11)

    Renamed FV_BOOLEAN as FV_TYPE_BOOLEAN... added some new ones.

$VER: 09.00 (2005/05/31)

    MORPHOS port by Guillaume ROGUEZ (yomgui1 [AT] gmail [DOT]com)

*/

/*

YOMGUI: si si ! recompte bien, on est bien à la 10.0 ! :)

GOFROMIEL: le temps passe tellement vite quand on s'amuse ;)

*/


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif
#ifndef FEELIN_COMPILER_H
#include <feelin/compiler.h>
#endif

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack(2)
#endif
#endif /* __MORPHOS__ */

/****************************************************************************
*** Basic *******************************************************************
****************************************************************************/

#define FF_TYPE_BOOLEAN                         (1 << 28)
#define FV_TYPE_BOOLEAN                         (FF_TYPE_BOOLEAN | 0)

#define FF_TYPE_INTEGER                         (1 << 29)

#define FV_TYPE_INTEGER                         (FF_TYPE_INTEGER | 0)
#define FV_TYPE_DECIMAL                         (FF_TYPE_INTEGER | 1)
#define FV_TYPE_HEXADECIMAL                     (FF_TYPE_INTEGER | 2)
#define FV_TYPE_BINARY                          (FF_TYPE_INTEGER | 3)
#define FV_TYPE_CONSTANT                        (FF_TYPE_INTEGER | 4)
#define FV_TYPE_PIXEL                           (FF_TYPE_INTEGER | 5)
#define FV_TYPE_PERCENTAGE                      (FF_TYPE_INTEGER | 6)

#define FF_TYPE_POINTER                         (1 << 30)

#define FV_TYPE_POINTER                         (FF_TYPE_POINTER | 0)
#define FV_TYPE_STRING                          (FF_TYPE_POINTER | 1)
#define FV_TYPE_OBJECT                          (FF_TYPE_POINTER | 2)

#define FF_TYPE_USER                            (1 << 31)

/****************************************************************************
*** Basic types *************************************************************
****************************************************************************/

typedef char                                    int8;
typedef short                                   int16;
typedef long                                    int32;
typedef unsigned char                           uint8;
typedef unsigned short                          uint16;
typedef unsigned long                           uint32;
typedef unsigned char                           bits8;
typedef unsigned short                          bits16;
typedef unsigned long                           bits32;

/*** less basics ones ******************************************************/

typedef void *                                  FObject;
typedef void                                    FNotifyHandler;

typedef unsigned long                           (ASM(*FHook))(REG(a0,struct Hook *Hook),REG(a2,FObject Obj),REG(a1,APTR Msg));
typedef unsigned long                           (ASM(*FHookEntry))(REG(a2,FObject Obj),REG(a1,APTR Msg));

/****************************************************************************
*** Nodes *******************************************************************
****************************************************************************/

typedef struct FeelinNode
{
    struct FeelinNode              *Next;
    struct FeelinNode              *Prev;
}
FNode;

typedef struct FeelinNodeData
{
    struct FeelinNodeData          *Next;
    struct FeelinNodeData          *Prev;
    APTR                            Data;
}
FNodeData;

typedef struct FeelinFamilyNode
{
    struct FeelinFamilyNode        *Next;
    struct FeelinFamilyNode        *Prev;
    FObject                         Object;
}
FFamilyNode;

/****************************************************************************
*** Lists *******************************************************************
****************************************************************************/

typedef struct FeelinList
{
    struct FeelinNode              *Head;
    struct FeelinNode              *Tail;
}
FList;

typedef struct FeelinListSemaphored
{
    struct FeelinNode              *Head;
    struct FeelinNode              *Tail;
//  end of FeelinList header
    struct SignalSemaphore          Semaphore;
}
FListSemaphored;

/****************************************************************************
*** Hashing *****************************************************************
****************************************************************************/

typedef struct FeelinHashLink
{
    struct FeelinHashLink          *Next;
    char                           *Key;
    uint32                          KeyLength;
    APTR                            Data;
}
FHashLink;

typedef struct FeelinHashTable
{
   uint32                           Size;
   FHashLink                      **Entries;
}
FHashTable;

#define FV_HASH_NORMAL                          211
#define FV_HASH_MEDIUM                          941
#define FV_HASH_HEAVY                           3911
#define FV_HASH_HEAVIER                         16267

/****************************************************************************
*** Classes *****************************************************************
****************************************************************************/

struct FeelinBase; /* defined in library/feelin.h */
struct FeelinClass;

typedef unsigned long (ASM (*FMethod))          (
                                                 REG(a2,APTR Class),
                                                 REG(a0,FObject Obj),
                                                 REG(d0,uint32 Method),
                                                 REG(a1,APTR Msg),
                                                 REG(a6,struct FeelinBase *FeelinBase)
                                                );

typedef struct FeelinDynamicEntry
{
    STRPTR                          Name;
    uint32                          ID;
}
FDynamicEntry;

typedef struct FeelinClassMethod
{
    FMethod                         Function;
    STRPTR                          Name;
    uint32                          ID;
}
FClassMethod;

typedef struct FeelinClassAttributeValue
{
    STRPTR                          Name;
    uint32                          Value;
}
FClassAttributeValue;

typedef struct FeelinClassAttribute
{
    STRPTR                          Name;
    uint32                          Type;
    uint32                          ID;
    FClassAttributeValue           *Values;
}
FClassAttribute;


typedef struct FeelinClass
{
    struct FeelinClass             *Next;
    struct FeelinClass             *Prev;
    struct FeelinClass             *Super;

    STRPTR                          Name;
    uint16                          Offset;
    uint16                          LODSize;
    APTR                            UserData;
    uint32                          UserCount;

    FClassMethod                   *Methods;
    FClassAttribute                *Attributes;
    FDynamicEntry                  *ResolvedIDs;
    FDynamicEntry                  *AutoResolvedIDs;
}
FClass;

typedef struct FeelinCatalogEntry
{
    uint32                          ID;
    STRPTR                          String;
    STRPTR                          Default;
}
FCatalogEntry;

/****************************************************************************
*** Threads *****************************************************************
****************************************************************************/

typedef struct FeelinThreadMessage
{
    struct Message                  SysMsg;
    uint16                          pad0;

    uint32                          Action;
    APTR                            Params;
    uint32                          Return;
}
FThreadMsg;

/****************************************************************************
*** Coordinates *************************************************************
****************************************************************************/

typedef struct FeelinBox
{
    int16                           x,y;
    uint16                          w,h;
}
FBox;

typedef struct FeelinRect
{
    int16                           x1,y1,x2,y2;
}
FRect;

typedef struct FeelinMinMax
{
    uint16                          MinW,MinH,MaxW,MaxH;
}
FMinMax;

typedef struct FeelinInner
{
    uint8                           l,t,r,b;
}
FInner;

/****************************************************************************
*** Display *****************************************************************
****************************************************************************/

typedef struct FeelinColor
{
    uint32                          _priv0;
    uint32                          _priv1;
    uint32                          Pen;
    uint32                          ARGB;
}
FColor;

typedef struct FeelinPalette
{
    uint32                          _priv0;
    uint32                          _priv1;
    uint32                          _priv2;
    uint32                          Count;
    uint32                         *Pens;
    uint32                         *ARGB;
    FColor                        **Colors;
}
FPalette;

/****************************************************************************
*** Render ******************************************************************
****************************************************************************/

typedef struct FeelinRender
{
    FObject                         Application;
    FObject                         Display;
    FObject                         Window;
    struct RastPort                *RPort;
    FPalette                       *Palette;
    bits32                          Flags;
}
FRender;

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif
