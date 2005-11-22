#ifndef _BTREENODES_H
#define _BTREENODES_H

#include <exec/types.h>
#include <libraries/iffparse.h>
#include "blockstructure.h"

/* Structures used by BNode trees */

struct BNode {
  ULONG key;
  ULONG data;
};

struct BTreeContainer {
  UWORD nodecount;
  UBYTE isleaf;
  UBYTE nodesize;   /* Must be a multiple of 2 */

  struct BNode bnode[0];
};

#define BNODECONTAINER_ID        MAKE_ID('B','N','D','C')

struct fsBNodeContainer {
  struct fsBlockHeader bheader;
  struct BTreeContainer btc;
};

#endif // _BTREENODES_H
