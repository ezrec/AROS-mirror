/*
 * @(#) $Header$
 *
 * BGUI library
 * makeproto.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:08:04  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:37  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/10/01 23:04:17  mlemos
 * Fixed problems with dangling pointers to missing command line arguments.
 *
 * Revision 41.10  1998/02/25 21:11:24  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:10  mlemos
 * Ian sources
 *
 *
 */

/*
 * Source is provided for demonstration and learning purposes only;
 * derivative products are a violation of international copyright.
 */

#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <string.h>
#include <ctype.h>

/*
 * MakeProto HEADER/K,SOURCE/M
 *
 * HEADER: The destination file to create, containing the prototype information.
 *         Include this file in all of your programs that need the global functions,
 *         the best place would be the main header file for your project.
 *
 * SOURCE: The file(s) to create prototype information for.  These files will be
 *         scanned for the "makeproto" keyword, which is case-sensitive, and will
 *         be defined to do nothing in the final compilation (this is done in the
 *         generated header file).
 *
 *         Unlike some other similar proto-building utilities, this one can handle
 *         split lines, to make large function declarations easier.  Also, there is
 *         no need to change the prototype in more than one place, you can use the
 *         "makeproto" keyword right in the definition.
 */

void killcomments(char *buffer)
{
   char *dest = buffer, c;

   int c_comment = 0, cpp_comment = 0;

   while (c = *buffer++)
   {
      switch (c)
      {
      case '/':
         switch (*buffer)
         {
         case '/':
            buffer++;
            cpp_comment++;
            break;
         case '*':
            buffer++;
            c_comment++;
            break;
         };
      case '*':
         if ((*buffer == '/') && (c_comment > 0))
         {
            buffer++;
            c_comment--;
            *dest++ = ' ';
         };
         break;
      case 10:
      case 13:
         cpp_comment = 0;
         break;
      };
      if (!(c_comment || cpp_comment)) *dest++ = c;
   };
   *dest = 0;
}

void makeproto(BPTR hf, char *buffer)
{
   BOOL protomade = FALSE, function;
   char c;
   char *brace, *equals, *semi, *start, *end, *comma;

   killcomments(buffer);

   while (buffer = strstr(buffer + 1, "makeproto"))
   {
      if ((buffer[-1] == ';') || (buffer[-1] == '*')) buffer[-1] = ' ';

      if (isspace(buffer[-1]) && isspace(buffer[9]))
      {
         start  = stpblk(buffer + 9);
         brace  = strchr(start, '{');
         equals = strchr(start, '=');
         semi   = strchr(start, ';');

         function = brace && (!semi || (brace < semi)) && (!equals || (brace < equals));

         if (!function)
         {
            if (equals && (equals < semi))
            {
               *equals = 0;
            }
            else
            {
               *semi = 0;
            };
            buffer = semi;

            strrev(start);
            while (isspace(*start)) start++;
            strrev(start);
         }
         else
         {
            *brace = 0;
            buffer = brace;

            comma = start;

            while (*comma)
            {
               if (CheckSignal(SIGBREAKF_CTRL_C)) return;

               if (end = strchr(comma, ','))
               {
                  comma = end;
               }
               else
               {
                  comma = strchr(comma, ')');
                  if (comma)
                  {
                     while (end = strchr(comma + 1, ')'))
                     {
                        comma = end;
                     };
                  };
               };

               if (!comma) return;
               c = *comma;
               *comma = 0;

               strrev(start);
               while (isspace(*start)) start++;
               if (strnicmp(start, "diov", 4))
               {
                  while (iscsym(*start)) start++;
                  while (isspace(*start)) start++;
               };
               strrev(start);

               *comma++ = c;

               if (c == ')')
               {
                  *comma = 0;
               }
               else
               {
                  end = comma;
                  while (isspace(*comma)) comma++;
                  if (comma != end)
                  {
                     *end++ = ' ';
                     if (comma != end) strcpy(end, comma);
                  };
               };
            };
         };
         protomade = TRUE;
         FPrintf(hf, "extern %s;\n", start);
      };
   };
   if (!protomade) FPrintf(hf, "/* No external prototypes defined. */\n");
}

int makeprotos(BPTR hf, char **sources)
{
   struct AnchorPath *ap;

   char *source, *buffer;
   BPTR  sf;

   if (ap = AllocVec(sizeof(struct AnchorPath) + 512, MEMF_CLEAR))
   {
      ap->ap_Strlen = 511;

      FPrintf(hf, " * Generated by MakeProto\n * ©1996 Ian J. Einman.\n */\n");

      while (source = *sources++)
      {
         if (!MatchFirst(source, ap))
         {
            do
            {
               if (sf = Open(ap->ap_Buf, MODE_OLDFILE))
               {
                  if (buffer = AllocVec(ap->ap_Info.fib_Size + 1, MEMF_CLEAR))
                  {
                     Read(sf, buffer, ap->ap_Info.fib_Size);
                  };
                  Close(sf);
                  if (buffer)
                  {
                     FPrintf(hf, "\n/* %-42s */\n\n", ap->ap_Info.fib_FileName);
                     makeproto(hf, buffer);
                     FreeVec(buffer);
                  };
               };
            }  while (!MatchNext(ap));
         };
         MatchEnd(ap);
      }
      FreeVec(ap);

      FPrintf(hf, "\n#define makeproto\n");
   }
   else
   {
      return 20;
   };
   return 0;
}

int main(void)
{
   int rc = 0;

   struct Args
   {
      char  *Header;
      char **Source;
   }  args;

   struct RDArgs *ra;
   BPTR   hf;

   args.Header=NULL;
   args.Source=NULL;
   if (ra = ReadArgs("HEADER/K,SOURCE/M", (LONG *)&args, NULL))
   {
   		if(args.Source)
   		{
         if (args.Header
         && (hf = Open(args.Header, MODE_NEWFILE)))
         {
            FPrintf(hf, "/*\n * %s\n *\n", args.Header);
            rc = makeprotos(hf, args.Source);
            Close(hf);
         }
         else if ((hf = Output()))
         {
            FPrintf(hf, "/*\n");
            rc = makeprotos(hf, args.Source);
         }
         else
         {
            rc = 20;
         };
      }
      else
      {
         rc = 20;
      };
      FreeArgs(ra);
   };
   return rc;
}
