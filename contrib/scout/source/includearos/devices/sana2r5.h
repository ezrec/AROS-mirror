#ifndef SANA2_SANA2R5_H
#define SANA2_SANA2R5_H 1

#ifndef SANA2_SANA2DEVICE_H
    #include <devices/sana2.h>
#endif

#ifndef S2_ADDMULTICASTADDRESSES
    #define S2_ADDMULTICASTADDRESSES  0xC000
#endif

#ifndef S2_DELMULTICASTADDRESSES
    #define S2_DELMULTICASTADDRESSES  0xC001
#endif

#ifndef S2_GETPEERADDRESS
    #define S2_GETPEERADDRESS         0xC002
#endif

#ifndef S2_GETDNSADDRESS
    #define S2_GETDNSADDRESS          0xC003
#endif

#ifndef S2_GETEXTENDEDGLOBALSTATS
    #define S2_GETEXTENDEDGLOBALSTATS 0xC004
#endif

#ifndef S2_CONNECT
    #define S2_CONNECT                0xC005
#endif

#ifndef S2_DISCONNECT
    #define S2_DISCONNECT             0xC006
#endif

#ifndef S2_SAMPLE_THROUGHPUT
    #define S2_SAMPLE_THROUGHPUT      0xC007
#endif

#ifndef S2_SANA2HOOK
    #define S2_SANA2HOOK              0xC008
#endif

#endif /* SANA2_SANA2R5_H */

