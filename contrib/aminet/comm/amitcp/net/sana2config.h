#ifndef NET_SANA2CONFIG_H
#define NET_SANA2CONFIG_H \
"$Id$"
/* 
 * Copyright © 1994 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * sana2config.h --- Configuration parameters for SANA-II network interfaces
 */

#ifndef DOS_RDARGS_H
#include <dos/rdargs.h>
#endif

#include <net/if_sana.h>

#define _PATH_SANA2CONFIG "AmiTCP:db/interfaces"

#define SSC_TEMPLATE \
  "NAME/A," \
  "DEV=DEVICE/A/K," \
  "UNIT/N/K," \
  "IPTYPE/N/K," \
  "ARPTYPE=IPARPTYPE/N/K," \
  "IPREQ=IPREQUESTS/N/K," \
  "ARPREQ=ARPREQUESTS/N/K," \
  "WRITEREQ=WRITEREQUESTS/N/K," \
  "NOTRACKING/S," \
  "NOARP/S," \
  "ARPHDR/N/K," \
  "P2P=POINT2POINT/S,NOSIMPLEX/S,LOOPBACK/S"

struct ssc_args {
  UBYTE *a_name;
  UBYTE *a_dev;
  LONG  *a_unit;
  LONG  *a_iptype;
  LONG  *a_arptype;
  LONG  *a_ipno;
  LONG  *a_arpno;
  LONG  *a_writeno;
  LONG   a_notrack;
  LONG   a_noarp;
  LONG  *a_arphdr;
  LONG   a_point2point;
  LONG   a_nosimplex;
  LONG   a_loopback;
};

struct ssconfig {
  LONG            flags;
  LONG            unit;
  char            name[IFNAMSIZ];
  struct RDArgs   rdargs[1];
  struct ssc_args args[1];
};

#define SSCF_RDARGS 1		/* set iff rdargs should be freed */

/*
 * Define how the interface database should be interpreted
 */
#define SSC_ALIAS  0
#define SSC_COMPAT 1

void ssconfig_free(struct ssconfig *config);
struct ssconfig *ssconfig_make(int how, char *name, long unit);
void ssconfig(struct sana_softc *ifp, struct ssconfig *ifc);

#endif /* !NET_SANA2CONFIG_H */
