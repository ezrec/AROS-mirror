/**  globals.h

            Written by Stephen Vermeulen (403) 282-7990

            PO Box 3295, Station B,
            Calgary, Alberta, CANADA, T2M 4L8

     here are the various globally known functions/variables/defines
 **/

extern int Enable_Abort;
extern struct IORequest *CreateExtIO();
extern void DeleteExtIO();
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct NewWindow new_cw;
extern struct Menu term_menu;

#define SIZE(a)  ((long) sizeof(struct a))

struct MySer
{
  struct MsgPort *writeport;  /** when the writeio is done, reply goes here **/
  struct MsgPort *readport;   /** ibid for the readio **/
  struct IOExtSer *readio, *writeio;
  UBYTE writedata[320], readdata[320];  /** data space **/
};

extern struct MySer *open_ser();
