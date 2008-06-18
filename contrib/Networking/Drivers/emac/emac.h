#ifndef EMAC_H_
#define EMAC_H_

#include <inttypes.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <devices/timer.h>
#include <devices/sana2.h>
#include <devices/sana2specialstats.h>

#define EMAC_TASK_NAME       "IBM EMAC task"
#define EMAC_PORT_NAME       "IBM EMAC port"

struct EMACUnit {
    struct SignalSemaphore      eu_Lock;
    struct MinList      eu_Openers;
    struct MinList      eu_MulticastRanges;
    struct MinList      eu_TypeTrackers;
    void                *eu_IRQHandler;
    uint8_t             eu_UnitNum;
    uint32_t            eu_Flags;
    int32_t             eu_RangeCount;
    
    void                    (*set_multicast)(struct EMACUnit *);
};

struct EMACBase {
    struct Device               emb_Device;
    struct Sana2DeviceQuery     emb_Sana2Info;
    void                        *emb_Pool;
    
    struct EMACUnit             *emb_Units[2];
};

/* Standard interface flags (netdevice->flags). */
#define IFF_UP          0x1             /* interface is up              */
#define IFF_BROADCAST   0x2             /* broadcast address valid      */
#define IFF_DEBUG       0x4             /* turn on debugging            */
#define IFF_LOOPBACK    0x8             /* is a loopback net            */
#define IFF_POINTOPOINT 0x10            /* interface is has p-p link    */
#define IFF_NOTRAILERS  0x20            /* avoid use of trailers        */
#define IFF_RUNNING     0x40            /* resources allocated          */
#define IFF_NOARP       0x80            /* no ARP protocol              */
#define IFF_PROMISC     0x100           /* receive all packets          */
#define IFF_ALLMULTI    0x200           /* receive all multicast packets*/

#define IFF_MASTER      0x400           /* master of a load balancer    */
#define IFF_SLAVE       0x800           /* slave of a load balancer     */

#define IFF_MULTICAST   0x1000          /* Supports multicast           */

#define IFF_VOLATILE    (IFF_LOOPBACK|IFF_POINTOPOINT|IFF_BROADCAST|IFF_MASTER|IFF_SLAVE|IFF_RUNNING)

#define IFF_PORTSEL     0x2000          /* can set media type           */
#define IFF_AUTOMEDIA   0x4000          /* auto media select active     */
#define IFF_DYNAMIC     0x8000          /* dialup device with changing addresses*/
#define IFF_SHARED      0x10000         /* interface may be shared */
#define IFF_CONFIGURED  0x20000         /* interface already configured */

enum {
    WRITE_QUEUE,
    ADOPT_QUEUE,
    EVENT_QUEUE,
    GENERAL_QUEUE,
    REQUEST_QUEUE_COUNT
};

struct Opener
{
    struct MinNode  node;
    struct MsgPort  read_port;
    BOOL            (*rx_function)(APTR, APTR, ULONG);
    BOOL            (*tx_function)(APTR, APTR, ULONG);
    struct Hook     *filter_hook;
    struct MinList  initial_stats;
};

struct TypeStats
{
    struct MinNode node;
    ULONG packet_type;
    struct Sana2PacketTypeStats stats;
};

struct TypeTracker
{
    struct MinNode node;
    ULONG packet_type;
    struct Sana2PacketTypeStats stats;
    ULONG user_count;
};

struct AddressRange
{
    struct MinNode node;
    ULONG add_count;
    ULONG lower_bound_left;
    ULONG upper_bound_left;
    UWORD lower_bound_right;
    UWORD upper_bound_right;
};

#define ETH_DATA_LEN            1500
#define ETH_ADDRESSSIZE         6
#define ETH_MTU                 (ETH_DATA_LEN)

void EMACIRQHandler(struct EMACBase *EMACBase, struct EMACUnit *Unit);
struct EMACUnit *CreateUnit(struct EMACBase *EMACBase, uint8_t num);

#endif /*EMAC_H_*/
