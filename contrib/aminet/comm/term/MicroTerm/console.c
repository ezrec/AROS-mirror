#include "symbols.y"
#define min(x,y) (x<y) ? x : y

/** console.c

            Written by Stephen Vermeulen (403) 282-7990

            PO Box 3295, Station B,
            Calgary, Alberta, CANADA, T2M 4L8

    this set of routines is all that is required to talk across
    the serial line, including a console device attached to an
    intuition window for user IO.
 **/

/** opens up the console device window so that we can talk
    to the modem...
 **/

struct Window *cw;                  /* console talk window */
struct IOStdReq *cwread, *cwwrite;  /** messages for console window **/
struct MsgPort *cwport;             /** message port for console window **/

BOOL open_con(void)
{
  cw = OpenWindow(&new_cw);
  if (cw)
  {
    SetMenuStrip(cw, &term_menu);
    cwport = CreatePort(NULL, 0L);
    if (cwport)
    {
      cwread = CreateStdIO(cwport);
      if (cwread)
      {
        cwwrite = CreateStdIO(cwport);
        if (cwwrite)
        {
          cwwrite->io_Data = (APTR) cw;
          cwwrite->io_Length = 4L;
          if (OpenDevice("console.device", 0L, (struct IORequest *)cwwrite, 0L))
          {
            /** failure **/
            DeleteStdIO(cwwrite);
          }
          else
          {
            /** success **/
            cwread->io_Device = cwwrite->io_Device;
            CopyMem(cwwrite, cwread, (long) sizeof(struct IOStdReq));
            return(TRUE);
          }
        }
        DeleteStdIO(cwread);
      }
      DeletePort(cwport);
    }
    CloseWindow(cw);
  }
  return(FALSE);
}

void close_con()
{
  CloseDevice((struct IORequest *)cwwrite);
  DeleteStdIO(cwwrite);
  DeleteStdIO(cwread);
  DeletePort(cwport);
  ClearMenuStrip(cw);
  CloseWindow(cw);
}

/** some buffers to hold data...
 **/

UBYTE conbuf[256];

void con_talk(ms)
struct MySer *ms;
{
  short num, i, more;
  long len;
  struct Message *emsg;
  struct IntuiMessage *msg;

  if (open_con());
  {
    /** do our intial reads from the console and serial devices to
        get the ball rolling...
     **/
    cwread->io_Command = CMD_READ;
    cwread->io_Flags = 0;
    cwread->io_Length = 255;
    cwread->io_Data = (APTR) conbuf;
    SendIO((struct IORequest *)cwread);
    ms->readio->IOSer.io_Command = CMD_READ;
    ms->readio->IOSer.io_Flags = 0;
    ms->readio->IOSer.io_Length = 1;
    ms->readio->IOSer.io_Data = (APTR) ms->readdata;
    SendIO((struct IORequest *)ms->readio);
    more = TRUE;
    while(more)
    {
      Wait( (1L << cw->UserPort->mp_SigBit)
          | (1L << cwport->mp_SigBit)
          | (1L << ms->readport->mp_SigBit) );
      if (NULL !=(emsg = GetMsg(cwport)))  /** key(s) pressed, so send to ser **/
      {
        ms->writeio->IOSer.io_Command = CMD_WRITE;
        ms->writeio->IOSer.io_Flags = 0;
        ms->writeio->IOSer.io_Length = cwread->io_Actual;
        CopyMem(conbuf, ms->writedata, (long) cwread->io_Actual);
        ms->writeio->IOSer.io_Data = (APTR) ms->writedata;
        DoIO((struct IORequest *)ms->writeio);  /** wait till serial device has sent it **/

        cwread->io_Command = CMD_READ;
        cwread->io_Flags = 0;
        cwread->io_Length = 255;
        cwread->io_Data = (APTR) conbuf;
        SendIO((struct IORequest *)cwread);
      }
      if (NULL != (emsg = GetMsg(ms->readport)))  /** serial info received, print it **/
      {
        /** first send the one lone byte we requested ages ago,
            then request any additional stuff that may be queued up.
         **/

        cwwrite->io_Command = CMD_WRITE;
        cwwrite->io_Flags = 0;
        cwwrite->io_Length = 1;
        cwwrite->io_Data = (APTR) ms->readdata;
        DoIO((struct IORequest *)cwwrite);
        /** ask for number of serial bytes queued up...
         **/

        ms->readio->IOSer.io_Command = SDCMD_QUERY;
        ms->readio->IOSer.io_Length = 0;
        DoIO((struct IORequest *)ms->readio);
        if (ms->readio->IOSer.io_Actual)
        {
          /** there were some bytes queued up at the serial device,
              so now ask for these...
           **/

          ms->readio->IOSer.io_Command = CMD_READ;
          len = ms->readio->IOSer.io_Actual;
          len = min(256L, len);
          ms->readio->IOSer.io_Length = len;
          ms->readio->IOSer.io_Flags = 0;
          ms->readio->IOSer.io_Data = (APTR) ms->readdata;
          ms->readio->IOSer.io_Error = 0;
          DoIO((struct IORequest *)ms->readio);
          cwwrite->io_Command = CMD_WRITE;
          cwwrite->io_Flags = 0;
          cwwrite->io_Length = len;
          cwwrite->io_Data = (APTR) ms->readdata;
          DoIO((struct IORequest *)cwwrite);
        }

        /** finally ask for another lone byte... **/

        ms->readio->IOSer.io_Command = CMD_READ;
        ms->readio->IOSer.io_Flags = 0;
        ms->readio->IOSer.io_Length = 1;
        ms->readio->IOSer.io_Data = (APTR) ms->readdata;
        SendIO((struct IORequest *)ms->readio);
      }
      while (msg = (struct IntuiMessage *) GetMsg(cw->UserPort))
      {
        if (msg->Class == MENUPICK)
        {
          num = msg->Code;
          while (num != (short)MENUNULL)
          {
            switch (MENUNUM(num))
            {
              case 0:
              switch (ITEMNUM(num))
              {
                case 0: /** does nothing **/
                  break;
                case 1: /** about **/
                  break;
                case 2: /** help **/
                  break;
                case 3: /** quit **/
                  more = FALSE;
                  break;
              }
              default:
                break;
            }
            num = ItemAddress(&term_menu, (long) num)->NextSelect;
          }
        }
        ReplyMsg((struct Message *)msg);
      }
    } /** end while(more) do **/
done_term:
    AbortIO((struct IORequest *)cwread);
    AbortIO((struct IORequest *)ms->readio);
    WaitIO((struct IORequest *)cwread);
    WaitIO((struct IORequest *)ms->readio);
    close_con();
  }
}
