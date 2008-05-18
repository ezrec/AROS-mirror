#ifndef EMAC_H_
#define EMAC_H_

#include <exec/libraries.h>
#include <exec/devices.h>
#include <devices/sana2.h>
#include <devices/sana2specialstats.h>

#define EMAC_TASK_NAME       "IBM EMAC task"
#define EMAC_PORT_NAME       "IBM EMAC port"

struct EMACBase {
    struct Device               emb_Device;
    struct Sana2DeviceQuery     emb_Sana2Info;
};

#define ETH_DATA_LEN            1500
#define ETH_ADDRESSSIZE         6
#define ETH_MTU                 (ETH_DATA_LEN)

#endif /*EMAC_H_*/
