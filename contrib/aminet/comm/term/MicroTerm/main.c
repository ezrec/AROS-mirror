#include "symbols.y"

/** Main.c  Written by Stephen Vermeulen (403) 282-7990

            PO Box 3295, Station B,
            Calgary, Alberta, CANADA, T2M 4L8

    This is the main entry point to a very simple terminal program
 **/

main(argc, argv)
int argc;
char *argv[];
{
  struct MySer *ms;
  char *sername;
  short serunit;

  FILE *out;

  if (argc > 1)  sername = argv[1];
    else sername = "serial.device";
  if (argc > 2)  serunit = atoi(argv[2]);
    else serunit = 0;
#warning Deactivated the following line
#if 0
  Enable_Abort = 0;
#endif
  puts("MicroTerm  0.1  By Stephen Vermeulen (403) 282-7990");

  if (open_libs())
  {
    if (ms = open_ser(sername, serunit))
    {
      con_talk(ms);
      close_ser(ms);
    }
  }
  close_libs();
}

