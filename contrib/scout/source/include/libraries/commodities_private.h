#ifndef LIBRARIES_COMMODITIES_PRIVATE_H
#define LIBRARIES_COMMODITIES_PRIVATE_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif

#ifndef EXEC_PORTS_H
#include <exec/ports.h>
#endif

#ifndef LIBRARIES_COMMODITIES_H
#include <libraries/commodities.h>
#endif

#if defined(__GNUC__)
# pragma pack(2)
#endif

/*-- PRIVATE FUNCTIONS --*/
CxObj *FindBroker( STRPTR name );
LONG CopyBrokerList( struct List *list );
LONG FreeBrokerList( struct List *list );
LONG BrokerCommand( STRPTR text, LONG id );

#if defined(__SASC)
    #pragma libcall CxBase FindBroker 6c 801        /* Private */
    #pragma libcall CxBase CopyBrokerList ba 801    /* Private */
    #pragma libcall CxBase FreeBrokerList c0 801    /* Private */
    #pragma libcall CxBase BrokerCommand c6 0802    /* Private */
#elif defined(__GNUC__)
    #if defined(__MORPHOS__)
        #define FindBroker(name) \
            LP1(0x6c, CxObj *, FindBroker, STRPTR, name, a1, \
            , COMMODITIES_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

        #define CopyBrokerList(list) \
            LP1(0xba, LONG, CopyBrokerList, struct List *, list, a0, \
            , COMMODITIES_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

        #define FreeBrokerList(list) \
            LP1(0xc0, LONG, FreeBrokerList, struct List *, list, a0, \
            , COMMODITIES_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

        #define BrokerCommand(text, id) \
            LP2(0xc6, LONG, BrokerCommand, STRPTR *, text, a0, LONG, id, d0, \
            , COMMODITIES_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)
    #elif !defined(__amigaos4__)
    	#ifndef __INLINE_MACROS_H
    	#include <inline/macros.h>
    	#endif
        #define FindBroker(name) \
            LP1(0x6c, CxObj *, FindBroker, STRPTR, name, a1, \
            , COMMODITIES_BASE_NAME)

        #define CopyBrokerList(list) \
            LP1(0xba, LONG, CopyBrokerList, struct List *, list, a0, \
            , COMMODITIES_BASE_NAME)

        #define FreeBrokerList(list) \
            LP1(0xc0, LONG, FreeBrokerList, struct List *, list, a0, \
            , COMMODITIES_BASE_NAME)

        #define BrokerCommand(text, id) \
            LP2(0xc6, LONG, BrokerCommand, STRPTR *, text, a0, LONG, id, d0, \
            , COMMODITIES_BASE_NAME)
    #endif
#endif

/*-- PRIVATE STRUCTURES --*/
struct BrokerCopy
{
    struct Node     bc_Node;                  // 00..0E
    char            bc_Name[CBD_NAMELEN];     // 0E..26
    char            bc_Title[CBD_TITLELEN];   // 26..4E
    char            bc_Descr[CBD_DESCRLEN];   // 4E..76
    struct Task *   bc_Task;                  // 76..7A
    struct MsgPort *bc_Port;                  // 7A..7E
    ULONG           bc_Dummy2;                // 7E..82   /* And another one nb_Unique */
    WORD            bc_Flags;                 // 82..84
};

/*-- PRIVATE CONSTANTS --*/
#define COF_ACTIVE 2


#if defined(__GNUC__)
# pragma pack(2)
#endif

#endif

