#ifndef PKTS_H
#define PKTS_H
/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

*/

struct pktinfo {
  struct MsgPort * fhType;
  struct MsgPort * port;
  struct DosPacket * pkt;
  char * bufs[2];
  int    whichbuf;
};

int initPktInfo(struct pktinfo * pkti, int fd,
		char * buf1, char * buf2, int size);
int deInitPktInfo(struct pktinfo * pkti);
int readPkt(struct pktinfo * pkti, char ** buf);
int writePkt(struct pktinfo * pkti, char ** buf, int len);

#endif /*  PKTS_H */
