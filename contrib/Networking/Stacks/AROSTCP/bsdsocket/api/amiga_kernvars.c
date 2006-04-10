#include <conf.h>

#include <exec/types.h>
#include <sys/types.h>
#include <api/amiga_kernvars.h>
#include <kern/amiga_netdb.h>

extern struct ifnet *ifnet;
extern struct icmpstat icmpstat;
extern struct radix_node_head *rt_tables[];

struct kernel_var kvars[] =
{
   { "_mbstat" , &mbstat  },
   { "_ipstat" , &ipstat  },
   { "_tcb"    , &tcb     },
   { "_tcpstat", &tcpstat },
   { "_udb"    , &udb     },
   { "_udpstat", &udpstat },
   { "_ifnet"  , &ifnet   },
   { "_icmpstat", &icmpstat },
   { "_rtstat"  , &rtstat   },
   { "_rt_tables", &rt_tables },
#if defined(ENABLE_IGMP)
   { "_igmpstat" , &igmpstat  },
#endif
#if defined(ENABLE_MULTICAST)
   { "_ip_mrtproto", &ip_mrtproto },
   { "_mrtstat"    , &mrtstat     },
   { "_mrttable"   , &mrttable    },
   { "_viftable"   , &viftable    },
#endif
   { "_NDB",       , &NDB         },
   { "_ndb_Lock"   , &ndb_Lock    },
   { "_DynDB"      , &DynDB       },
   { "_ndb_Serial" , &ndb_Serial  },
   { NULL, NULL }
};

void * SAVEDS FindKernelVar(REG(A0, STRPTR name))
{
   struct kernel_var *kvar = NULL;
   for (kvar = kvars; kvar->v_name; kvar++)
   {
      if (!(strcmp(name, kvar->v_name))) return kvar->v_addr;
   }
   return NULL;
}
