/* NTPSync by Heikki Linnakangas

  Distribution of modified versions of the program is generally prohibited.
  If you  make any enhancements to program, please send the source code to
  me, and I will make a new release. Feel free to experiment!

  NTPSync was compiled with SAS/C compiler V6.50 and AmiTCP SDK 4.0 with the
  following line:

  sc NTPSync lib:net.lib NOICONS UTILLIB NOSTKCHK LINK SCODE SDATA NOCHKABORT STRIPDBG

  Sorry, the code is very messy and poorly commented. Oh well. If you want
  to clean it up, go ahead, and please send the cleaned up version to me too :)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if !defined(__GNUC__)
#pragma amiga-align
#endif

#include <exec/memory.h>
#include <exec/libraries.h>
#include <devices/timer.h>
#include <resources/battclock.h>
#include <proto/exec.h>
#if defined(__AROS__)
#define __LOCALE_STDLIBBASE__
#endif
#include <proto/locale.h>
#include <proto/timer.h>
#if defined(__AROS__)
#define __BATTCLOCK_STDLIBBASE__
#endif
#include <proto/battclock.h>
#ifdef __amigaos4__
#include <proto/bsdsocket.h>
#else
#include <proto/socket.h>
#endif
#include <clib/alib_protos.h>
#include <clib/macros.h>
#if !defined(__GNUC__)
#pragma default-align
#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define NTP_PORT 123

#define VERSION "2.3"
#define RELDATE "03.02.06"

UBYTE *version = "$VER: NTPSync "VERSION" ("RELDATE")";

struct Timestamp {
  unsigned long seconds;
  unsigned long fractions;
};

struct ntp_message {
  unsigned char Mode;
  unsigned char Stratum;
  unsigned char Poll;
  unsigned char Precision;
  long RootDelay;
  long RootDispesion;
  long ReferenceIdentifier;
  struct Timestamp ReferenceTimestamp;
  struct Timestamp OriginateTimestamp;
  struct Timestamp ReceiveTimestamp;
  struct Timestamp TransmitTimestamp;
};
struct ntp_address {
  struct ntp_address *next;
  unsigned long addr;
  unsigned short port;
  char *name;
};

#define UTCNOTSET 0xFFFFFFFF

int UTCdiff = UTCNOTSET;

#if defined(__AROS__)
struct Library *SocketBase;
#endif

/* Calculates dst = s1+s2, src must be != dst
*/
void TSAdd(struct Timestamp *s1,struct Timestamp *s2, struct Timestamp *dst)
{
  dst->fractions = s1->fractions+s2->fractions;

  if(dst->fractions < MAX(s1->fractions,s2->fractions)) {
    dst->seconds = s1->seconds + s2->seconds + 1;
  } else 
    dst->seconds = s1->seconds + s2->seconds;
}

/* Calculates dst = s1-s2, s1 must be >s2
*/
void TSSub(struct Timestamp *s1,struct Timestamp *s2, struct Timestamp *dst)
{
  if(s1->fractions<s2->fractions) {
    dst->seconds = s1->seconds - s2->seconds - 1;
  } else {
    dst->seconds = s1->seconds - s2->seconds;
  }
  dst->fractions = s1->fractions - s2->fractions;
}

/* Calculates dst = src/2, src must be != dst
*/
void TSHalv(struct Timestamp *src, struct Timestamp *dst)
{
  dst->seconds   = src->seconds>>1;
  dst->fractions = src->fractions>>1;
  if(src->seconds&1) {
    dst->fractions |= 1<<31;
  }
}

void Ami2NTP(struct timeval *tv,struct Timestamp *ts)
{
  ts->seconds = tv->tv_secs+2461449600UL-UTCdiff;
  ts->fractions = (tv->tv_micro*4295)|1;
}
void NTP2Ami(struct timeval *tv,struct Timestamp *ts)
{
  tv->tv_secs = ts->seconds-2461449600UL+UTCdiff;
  tv->tv_micro = ts->fractions/4295;
}

void printhelp(int i)
{
  puts("NTPSync version "VERSION" released at "RELDATE);
  puts("\n   Usage: NTPSync [options] <1st server>[:port] [2nd server][:port] [...]");
  puts("\nWhere options is one or more of the following:");
  puts("    -h          = this help");
  puts("    -v          = verbose mode");
  puts("    -c          = set the battery-backed clock too on success");
  puts("    -t<t>       = timeout of network functions in seconds");
  puts("    -d<UTCDiff> = difference from UTC in minutes\n");
  exit(i);
}

/* Returns -1 if the offset is negative, 1 otherwise */
int CalculateOffset(struct Timestamp *O, struct Timestamp *R, struct Timestamp *T,
                    struct Timestamp *D, struct Timestamp *dst)
{
  struct Timestamp t1,t2,t3;
  int polarity;

  /* t = ((R - O) + (T - D))/2 = (R + T - (O + D))/2 */

  TSAdd(R,T,&t1);
  TSAdd(O,D,&t2);

  if(t1.seconds==t2.seconds) {
    if(t1.fractions>t2.fractions) {
      polarity = +1;
      TSSub(&t1,&t2,&t3);
    } else {
      polarity = -1;
      TSSub(&t2,&t1,&t3);
    }
  } else if(t1.seconds>t2.seconds) {
    polarity = +1;
    TSSub(&t1,&t2,&t3);
  } else {
    polarity = -1;
    TSSub(&t2,&t1,&t3);
  }
  TSHalv(&t3,dst);

  return(polarity);
}
void CalculateRoundtrip(struct Timestamp *O, struct Timestamp *R, struct Timestamp *T,
                        struct Timestamp *D, struct Timestamp *dst)
{
  struct Timestamp t1,t2;

  /* d = (D - O) - (R - T) */

  TSSub(D,O,&t1);
  TSSub(R,T,&t2);
  TSSub(&t1,&t2,dst);
}

void freelist(struct ntp_address *first)
{
  struct ntp_address *a;

  while(first) {
    a=first->next;
    free(first);
    first=a;
  }
}

struct Library *LocaleBase;
struct Library *BattClockBase = 0;
#ifdef __amigaos4__
struct LocaleIFace *ILocale;
struct BattClockIFace *IBattClock;
#endif


int main(int argc,char *argv[])
{
  int sockfd;
  struct ntp_message msg;
  int retval=0;
  int i;
  struct Timestamp DestinationTimestamp,tmp1,tmp2;
  struct timerequest *GetTimeIO, *SetTimeIO;
  struct MsgPort *TimerMP;
  int verbose=0;
  struct ntp_address *firstserver=0,*a;
  int timeout=0;

#if defined(__AROS__)
  if((SocketBase = OpenLibrary("bsdsocket.library",3)) == NULL)
  {
    puts("\n[NTPSync] Couldn't open bsdsocket.library\n");
    exit(0);
  }
#endif
	
  if(argc<2) {
    printhelp(20);
  }

  /* Parse command-line options */
  for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"?")) {
      freelist(firstserver);
      printhelp(0);
    } else {
      if(!strcmp(argv[i],"-v")) {
        verbose=1;
      } else {
        if(!strncmp(argv[i],"-d",2)) {
          if(UTCdiff!=UTCNOTSET || argv[i][2]==0) {
            freelist(firstserver);
            printhelp(20);
          } else {
            UTCdiff = atoi(&argv[i][2])*60;
            /* Just a quick sanity check */
            if(UTCdiff<10000 && UTCdiff>10000) {
              freelist(firstserver);
              printhelp(20);
            }
          }
        } else {
          if(!strncmp(argv[i],"-t",2)) {
            if(timeout || argv[i][2]==0) {
              freelist(firstserver);
              printhelp(20);
            } else {
              timeout = atoi(&argv[i][2]);
              /* Just a quick sanity check */
              if(timeout<1) {
                freelist(firstserver);
                printhelp(20);
              }
            }
          } else {
            if(!strcmp(argv[i],"-c")) {
              if(!(BattClockBase = OpenResource(BATTCLOCKNAME))) {
                puts("Can't open "BATTCLOCKNAME"!");
                freelist(firstserver);
                exit(15);
              }
#ifdef __amigaos4__
              if (!(IBattClock = (struct BattClockIFace *)GetInterface((struct Library *)BattClockBase,"main",1,0))) {
                puts("Can't get Battclock interface!");
                freelist(firstserver);
                exit(15);
              }
#endif
            } else {
              unsigned int port;
              struct hostent *hent;
              char name[80];
              int j;

              for(j=0;argv[i][j] && argv[i][j]!=':';j++) {
                name[j]=argv[i][j];
              }
              name[j]=0;
              if(argv[i][j]==':') {
                port=atoi(&argv[i][j+1]);
              } else port=NTP_PORT;

              if(!(hent=gethostbyname(name))) {
                printf("Can't resolve address %s!\n",argv[i]);
              } else {
                struct ntp_address *addr;
                if(addr = malloc(sizeof(struct ntp_address))) {
                  addr->next = firstserver;
                  firstserver = addr;
                  addr->addr = *((unsigned long *)hent->h_addr);
                  addr->port = htons(port);
                  addr->name = argv[i];
                } else {
                  puts("Can't allocate memory for address! (Out of memory?)");
                }
              }
            }
          }
        }
      }
    }
  }
  if(!firstserver) {
    puts("No valid servers given!");
    return(10);
  }

  if(UTCdiff == UTCNOTSET) {
    struct Locale *locale;

    if(LocaleBase = OpenLibrary("locale.library",0)) {
#ifdef __amigaos4__
      if(!(ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase,"main",1,0))) {
        CloseLibrary(LocaleBase);
        puts("Can't get Locale interface!");
        freelist(firstserver);
        return(12);
      }
#endif
      if(locale = OpenLocale(NULL)) {
        UTCdiff = -locale->loc_GMTOffset*60;
        if(verbose)
          printf("UTC difference not given, locale.library told %ld mins...\n",(long)-locale->loc_GMTOffset);
        CloseLocale(locale);
      } else {
        retval=11;
        puts("Can't open locale!");
      }
#ifdef __amigaos4__
      DropInterface((struct Interface *)ILocale);
#endif
      CloseLibrary(LocaleBase);
    } else {
      retval=12;
      puts("Can't open locale.library!");
    }
    if(retval) {
      freelist(firstserver);
      return(retval);
    }
  }
  if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
    puts("Can't create socket!");
    retval=10;
  } else {
    struct sockaddr_in cli_addr;

    memset((char *)&cli_addr,0,sizeof(cli_addr));
    cli_addr.sin_family      = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port        = htons(NTP_PORT);

    if(bind(sockfd, (struct sockaddr *) &cli_addr,sizeof(cli_addr)) < 0 ) {
      cli_addr.sin_port        = htons(0);
      if(bind(sockfd, (struct sockaddr *) &cli_addr,sizeof(cli_addr)) < 0 ) {
        puts("Can't bind()!");
        retval=9;
      }
    }
    if(timeout && !retval) {
      struct timeval tv;

      tv.tv_secs  = timeout;
      tv.tv_micro = 0;

      if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(struct timeval))) {
        puts("Can't set timeout!");
        freelist(firstserver);
        retval=12;
      }
      if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval))) {
        puts("Can't set timeout!");
        freelist(firstserver);
        retval=12;
      }
    }
    if(!retval) {
      if(TimerMP = CreateMsgPort()) {
        if(GetTimeIO = (struct timerequest *) CreateExtIO(TimerMP,sizeof(struct timerequest))) {
          if(!(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)GetTimeIO,0L))) {
            if(SetTimeIO = (struct timerequest *) AllocMem(sizeof(struct timerequest),MEMF_PUBLIC | MEMF_CLEAR)) {
              int fromlen;
              struct sockaddr_in serv_addr;

              *SetTimeIO = *GetTimeIO;

              GetTimeIO->tr_node.io_Command = TR_GETSYSTIME;
              SetTimeIO->tr_node.io_Command = TR_SETSYSTIME;

              memset((char *)&msg,0,sizeof(struct ntp_message));
              msg.Mode = 0x13;  /* VN=2, Mode=3 (client) */

              fromlen = sizeof(serv_addr);

              memset((char *)&serv_addr,0,sizeof(serv_addr));
              serv_addr.sin_family      = AF_INET;

              if(verbose)
                puts("Sending requests, and waiting for a reply...");

              DoIO((struct IORequest *) GetTimeIO);
              Ami2NTP(&GetTimeIO->tr_time,&msg.TransmitTimestamp);

              for(a=firstserver;a;a=a->next) {
                serv_addr.sin_addr.s_addr = a->addr;
                serv_addr.sin_port = a->port;

                sendto(sockfd,(char *)&msg,sizeof(msg),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
              }
              retval = recvfrom(sockfd,(char *)&msg,sizeof(msg),0,(struct sockaddr *)&serv_addr,(LONG *)&fromlen);

              DoIO((struct IORequest *) GetTimeIO);
              Ami2NTP(&GetTimeIO->tr_time,&DestinationTimestamp);

              if(retval>=sizeof(struct ntp_message) && (msg.Mode&7)==4 && (msg.Mode&0x38)<0x28) {
                int polarity;

                polarity=CalculateOffset(&msg.OriginateTimestamp,&msg.ReceiveTimestamp,
                                         &msg.TransmitTimestamp,&DestinationTimestamp,&tmp1);
                if(polarity==1) {
                  TSAdd(&DestinationTimestamp,&tmp1,&tmp2);
                } else TSSub(&DestinationTimestamp,&tmp1,&tmp2);

                NTP2Ami(&SetTimeIO->tr_time,&tmp2);
                if(BattClockBase)
                  WriteBattClock(SetTimeIO->tr_time.tv_secs);
                DoIO((struct IORequest *) SetTimeIO);

                if(verbose) {
                  /* The time has been set, no hurry anymore */
                  CalculateRoundtrip(&msg.OriginateTimestamp,&msg.ReceiveTimestamp,
                                     &msg.TransmitTimestamp,&DestinationTimestamp,&tmp2);
                  tmp1.fractions=tmp1.fractions/4295;
                  tmp2.fractions=tmp2.fractions/4295;

                  for(a=firstserver;a;a=a->next) {
                    if(a->addr==serv_addr.sin_addr.s_addr && serv_addr.sin_port == a->port) break;
                  }

                  printf("Time set according to ");
                  if(!a) {
                    printf("*** an unknown server ***");
                  } else {
                    printf("%s",a->name);
                  }
                  printf(" (Stratum %u):\n",(unsigned)msg.Stratum);
                  printf("Roundtrip delay %lu µs, offset",tmp2.seconds*100000+tmp2.fractions);
                  if(tmp1.seconds>365*24*60*60) {
                    printf(" %lu years",tmp1.seconds/(365*24*60*60));
                    tmp1.seconds = tmp1.seconds%(365*24*60*60);
                  }
                  if(tmp1.seconds>24*60*60) {
                    printf(" %lu days",tmp1.seconds/(24*60*60));
                    tmp1.seconds = tmp1.seconds%(24*60*60);
                  }
                  if(tmp1.seconds>60*60) {
                    printf(" %lu hours", tmp1.seconds/(60*60));
                    tmp1.seconds = tmp1.seconds%(60*60);
                  }
                  if(tmp1.seconds>60) {
                    printf(" %lu minutes", tmp1.seconds/(60));
                    tmp1.seconds = tmp1.seconds%(60);
                  }
                  if(tmp1.seconds>0) {
                    printf(" %lu seconds", tmp1.seconds);
                  }
                  if(tmp1.fractions>0) {
                    printf(" %lu microseconds",tmp1.fractions);
                  }
                  if(polarity==1) {
                    puts(" forwards.");
                  } else {
                    puts(" backwards.");
                  }
                  if(BattClockBase)
                    puts("The battery-backed clock was also set.");
                }
                retval=0;
              } else {
                puts("Got an invalid reply from a server (or error while receiving), time not set!");
                retval=5;
              }
              FreeMem(SetTimeIO,sizeof(struct timerequest));
            }
            CloseDevice((struct IORequest *)GetTimeIO);
          } else {
            puts("Error in allocating resources!");
            retval=6;
          }
          DeleteExtIO((struct IORequest *)GetTimeIO);
        } else {
          puts("Error in allocating resources!");
          retval=7;
        }
        DeleteMsgPort(TimerMP);
      } else {
        puts("Error in allocating resources!");
        retval=8;
      }
    }
    CloseSocket(sockfd);
  }
#ifdef __amigaos4__
  if(BattClockBase)
    DropInterface((struct Interface *)IBattClock);
#endif
  freelist(firstserver);
  return(retval);
}
