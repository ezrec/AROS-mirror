/* Prototypes for functions defined in traceroute.c */

extern u_char  packet[512];
extern struct  opacket * outpacket;
extern int     s;
extern int     sndsock;
extern struct  timezone tz;
extern struct  sockaddr whereto;
extern int     datalen;
extern char   *source;
extern char   *hostname;
extern char    hnamebuf[MAXHOSTNAMELEN];
extern int     nprobes;
extern int     max_ttl;
extern u_short ident;
extern u_short port;
extern int     options;
extern int     verbose;
extern int     waittime;
extern int     nflag;


int   wait_for_reply(int sock, struct sockaddr_in * from);
void  send_probe(int seq, int ttl);
int   deltaT(struct timeval *tp);
int   packet_ok(u_char * buf, int cc, struct sockaddr_in * from, int seq); 
void  print(u_char * buf, int cc, struct sockaddr_in * from);
void  tvsub(register struct timeval * out, register struct timeval * in);
char *inetname(struct in_addr in);

