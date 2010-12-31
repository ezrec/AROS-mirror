/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#define __USE_OLD_TIMEVAL__
#include "system_headers.h"

extern APTR    AP_Scout;
extern STRPTR  _ProgramName;

/********************************************************************/
/*                      Benötigte Prototypes                        */
/********************************************************************/

#define NAMELEN      32
#define LINELEN      82
#define TRENNCHAR    '\0'

CONST_STRPTR CMD_BEGIN = "BEGIN";
CONST_STRPTR CMD_USER = "USER";
CONST_STRPTR CMD_PASSWORD = "PASSWORD";
CONST_STRPTR CMD_END = "END";
CONST_STRPTR CMD_DONE = "DONE";
CONST_STRPTR CMD_ERROR = "ERROR";
CONST_STRPTR CMD_CONNECTED = "CONNECTED";

STATIC struct RDArgs *rdargs;
STATIC struct RDArgs *myrdargs;

STATIC int server_socket = -1;
STATIC int client_socket = -1;
STATIC int connected = FALSE;
STATIC long type = SOCK_STREAM;

STATIC LONG sinlen;
STATIC struct sockaddr_in sin;

BOOL clientstate = FALSE;
BOOL serverstate = FALSE;
BOOL shellstate = FALSE;

char username[10+1];
char password[_PASSWORD_LEN+1];

#if defined(__amigaos4__)
struct Library*         SocketBase;
struct SocketIFace*     ISocket;
struct Library*         UserGroupBase;
struct UserGroupIFace*  IUserGroup;
#endif

#if defined(__AROS__)
struct Library*         SocketBase;
struct Library*         UserGroupBase;
#endif

extern int init_inet_daemon(void);

void failtcp( void )
{
    if (client_socket >= 0) {
        if (connected) shutdown(client_socket, 2);
        CloseSocket(client_socket);
    }

    if (UserGroupBase) {
        DROPINTERFACE(IUserGroup);
        CloseLibrary(UserGroupBase);
    }

    if (SocketBase) {
        DROPINTERFACE(ISocket);
        CloseLibrary(SocketBase);
    }
}

int inittcp( void )
{
    if ((SocketBase = OpenLibrary("bsdsocket.library", 4L)) == NULL) {
        return FALSE;
    }
    if (!GETINTERFACE(ISocket, SocketBase)) {
        return FALSE;
    }

    if ((UserGroupBase = OpenLibrary("usergroup.library", 4)) == NULL) {
        return FALSE;
    }
    if (!GETINTERFACE(IUserGroup, UserGroupBase)) {
        return FALSE;
    }

    return TRUE;
}

STATIC int sgetc( int sock )
{
    unsigned char c;
    fd_set rd,ex;
    ULONG flgs;
    int n;
    struct timeval t;

    t.tv_sec = 10L;
    t.tv_usec = 0;

    FD_ZERO(&rd);
    FD_ZERO(&ex);

    FD_SET(sock,&rd);
    FD_SET(sock,&ex);

    if (clientstate) {
        flgs = 0;
        WaitSelect(16,&rd,0L,&ex,&t,&flgs);
    } else {
        flgs = SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D;
        WaitSelect(16,&rd,0L,&ex,NULL,&flgs);
    }

    if (FD_ISSET(sock,&rd)) {
        n = recv(sock, &c, 1, 0);
        if (n == 1) {
            return c;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

int sgets( int sock,
           STRPTR string,
           int maxchars )
{
    int nchars = 0;
    char c;

    while (nchars < maxchars && (c = sgetc(sock)) != (char)EOF && c != TRENNCHAR) {
        string[nchars] = c;
        nchars++;
    }
    string[nchars] = '\0';
    return nchars;
}

STATIC CONST_STRPTR NetHelpText = "\n" \
    " Available lists:\n" \
    " ----------------\n" \
    " Allocations     Assigns         BoopsiClasses   Commands\n" \
    " Commodities     Devices         Expansions      Fonts\n" \
    " InputHandlers   Interrupts      Libraries       Locks\n" \
    " LowMemory       Memory          Mounts          Ports\n" \
    " Residents       Resources       ScreenMode      Semaphores\n" \
    " System          Tasks           Timer           Vectors\n" \
    " Windows\n\n" \
    " Other commands:\n" \
    " ---------------\n" \
    " Help";

void PrintNetHelp( STRPTR UNUSED dummy )
{
    if (serverstate || shellstate) {
        int i = 0;
        STRPTR tmp;

        if ((tmp = AllocVec(256, MEMF_ANY)) != NULL) {
            SendResultString(NetHelpText);

            while (arexx_list[i].mc_Name) {
                if (arexx_list[i].mc_Parameters) {
                    _snprintf(tmp, 256, " %s %s", arexx_list[i].mc_Name, arexx_list[i].mc_Template);
                } else {
                    _snprintf(tmp, 256, " %s", arexx_list[i].mc_Name);
                }
                i++;
            }

            FreeVec(tmp);
        }
    }
}

struct Net_Command {
    CONST_STRPTR nc_Name;
    void (* nc_func)( STRPTR );
};

STATIC struct Net_Command net_list[] = {
    { "Allocations",         PrintAllocations },
    { "Assigns",             PrintAssigns },
    { "BoopsiClasses",       PrintClass },
    { "Commands",            PrintCommands },
    { "Commodities",         PrintCx },
    { "Devices",             PrintDevices },
    { "Expansions",          PrintExpansions },
    { "Fonts",               PrintFonts },
    { "InputHandlers",       PrintInputHandlers },
    { "Interrupts",          PrintInterrupts },
    { "Libraries",           PrintLibraries },
    { "LowMemory",           PrintLowMemory },
    { "Memory",              PrintMemory },
    { "Mounts",              PrintMounts },
    { "Locks",               PrintLocks },
    { "Ports",               PrintPorts },
    { "Residents",           PrintResidents },
    { "ScreenModes",         PrintSMode },
    { "Semaphores",          PrintSemaphores },
    { "System",              PrintSystem },
    { "Tasks",               PrintTasks },
    { "Timer",               PrintTimer },
    { "Resources",           PrintResources },
    { "Vectors",             PrintVectors },
    { "Windows",             PrintWindows },
    { "Patches",             PrintPatches },
    { "Catalogs",            PrintCatalogs },
    { "AudioModes",          PrintAudioModes },
    { "ResetHandlers",       PrintResetHandlers },
    { "Help",                PrintNetHelp },
    { "a",                   PrintAllocations },
    { "b",                   PrintClass },
    { "c",                   PrintCommands },
    { "d",                   PrintDevices },
    { "e",                   PrintTimer },
    { "f",                   PrintFonts },
    { "g",                   PrintAssigns },
    { "h",                   PrintInputHandlers },
    { "i",                   PrintInterrupts },
    { "j",                   PrintLowMemory },
    { "k",                   PrintCx },
    { "l",                   PrintLibraries },
    { "m",                   PrintMemory },
    { "n",                   PrintMounts },
    { "o",                   PrintLocks },
    { "p",                   PrintPorts },
    { "r",                   PrintResidents },
    { "s",                   PrintSemaphores },
    { "t",                   PrintTasks },
    { "u",                   PrintResources },
    { "v",                   PrintVectors },
    { "w",                   PrintWindows },
    { "x",                   PrintExpansions },
    { "y",                   PrintSystem },
    { "z",                   PrintSMode },
    { "GetAlcList",          SendAlcList },
    { "GetAssList",          SendAssList },
    { "GetClassList",        SendClassList },
    { "GetComList",          SendComList },
    { "GetCxList",           SendCxList },
    { "GetDevList",          SendDevList },
    { "GetExpList",          SendExpList },
    { "GetFontList",         SendFontList },
    { "GetInputList",        SendInputList },
    { "GetIntList",          SendIntList },
    { "GetLibList",          SendLibList },
    { "GetLockList",         SendLockList },
    { "GetLowMemList",       SendLowMemory },
    { "GetMemList",          SendMemList },
    { "GetMountList",        SendMountList },
    { "GetPortList",         SendPortList },
    { "GetResList",          SendResList },
    { "GetResiList",         SendResiList },
    { "GetSemList",          SendSemList },
    { "GetSModeList",        SendSModeList },
    { "GetSysList",          SendSystemList },
    { "GetTaskList",         SendTaskList },
    { "GetTimerList",        SendTimerList },
    { "GetVectorList",       SendVectorList },
    { "GetWinList",          SendWinList },
    { "GetPatchList",        SendPatchList },
    { "GetCatalogList",      SendCatalogList },
    { "GetAudioModeList",    SendAudioModeList },
    { "GetResetHandlerList", SendResetHandlersList },
    { NULL,                  NULL },
};

/********************************************************************/
/*                              Client                              */
/********************************************************************/

int VARARGS68K STDARGS SendDaemon( CONST_STRPTR fmt, ... )
{
    int result = FALSE;
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, VA_ARG(args, APTR))) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        if (send(client_socket, buf, strlen(buf) + 1, 0) == (LONG)strlen(buf) + 1) {
            result = TRUE;
        }

        FreeVec(buf);
    }

    VA_END(args);

    return result;
}

STATIC ULONG NetCommand( STRPTR text )
{
    ULONG rc = RETURN_FAIL;
    STRPTR buf;

    if ((buf = tbAllocVecPooled(globalPool, LINELEN)) != NULL) {
        if (send(client_socket, text, strlen(text) + 1, 0) == (LONG)strlen(text) + 1) {
            while (sgets(client_socket, buf, LINELEN)) {
                if (strcmp(buf, CMD_DONE) == 0) {
                    rc = RETURN_OK;
                    break;
                } else if (strcmp(buf, CMD_ERROR) == 0) {
                    break;
                }
            }
        }

        tbFreeVecPooled(globalPool, buf);
    }

    return rc;
}

BOOL ReceiveDecodedEntry( void *structure,
                          ULONG length )
{
    BOOL rc = FALSE;

    if (sgets(client_socket, structure, length)) {
        if (strcmp(structure, CMD_ERROR) != 0 && strcmp(structure, CMD_DONE) != 0) {
            STRPTR s = (STRPTR)structure;
            ULONG i;

            for (i = 0; i < length; i++) {
                if (s[i] == '\1') s[i] = '\0';
            }

            rc = TRUE;
        }
    }

    return rc;
}

int ConnectToServer( void )
{
   TEXT hostname[NAMELEN];
   TEXT buffer[LINELEN+2];
   struct hostent *host;
   struct sockaddr_in server;

   if (!inittcp()) {
       PutStr(msgCantInitTCPIP);
       return FALSE;
   }

   if (opts.User) {
       stccpy(username, opts.User, sizeof(username));
   } else {
       stccpy(username, getlogin(), sizeof(username));
   }

   if (opts.Password) {
       stccpy(password, opts.Password, sizeof(password));
   } else {
       stccpy(password, getpass(msgAskForPassword), sizeof(password));
       if (password[0] == '\0') {
           PutStr(msgCantGetPassword);
           return FALSE;
       }
   }

   gethostname(hostname, sizeof(hostname));

   if ((host = gethostbyname(opts.Host)) != NULL) {
       bzero((char *)&server, sizeof(server));
       server.sin_port = 6543;
       bcopy(host->h_addr, (char *)&server.sin_addr, host->h_length);
       server.sin_family = host->h_addrtype;

       client_socket = socket(AF_INET, type, 0);
       if (client_socket >= 0) {
           if (connect (client_socket, (struct sockaddr *) &server, sizeof (server)) != -1) {
               connected = TRUE;
               if (SendDaemon(CMD_BEGIN)) {
                   if (SendDaemon("%s %s", CMD_USER, username) && SendDaemon("%s %s", CMD_PASSWORD, password)) {
                       if (sgets(client_socket, buffer, LINELEN) && strcmp(buffer, CMD_CONNECTED) != 0) {
                          PutStr(buffer);
                      } else {
                          return TRUE;
                      }
                   }
               }
           } else {
              PutStr(msgCantConnectToServer);
           }
       } else {
           PutStr(msgCantCreateSocket);
       }
   } else {
       Printf(msgCantFindHost, opts.Host);
   }
   return FALSE;
}

ULONG netshellclient( void )
{
    ULONG rc = RETURN_FAIL;
    TEXT recvbuffer[LINELEN+2];

    if ((clientstate = ConnectToServer())) {
        if (SendDaemon("%s ", opts.Command)) {
            while (sgets(client_socket, recvbuffer, LINELEN) && strcmp(recvbuffer, CMD_ERROR) != 0 && strcmp(recvbuffer, CMD_DONE) != 0) {
               PutStr(recvbuffer);
            }
            if (strcmp(recvbuffer, CMD_DONE) == 0) {
                rc = RETURN_OK;
            } else {
                rc = RETURN_ERROR;
            }
            SendDaemon(CMD_END);
        }
    }

    return rc;
}

/********************************************************************/
/*                              Daemon                              */
/********************************************************************/

int isNetCall (void)
{
   int   rc = TRUE;

   if (! inittcp()) {
      return (FALSE);
   }

   server_socket = init_inet_daemon();
   if (server_socket >= 0) {
      sinlen = sizeof (sin);
      if (getpeername (server_socket, (struct sockaddr *) &sin, &sinlen) == -1) {
//         logprint ("scout: getpeername() failed\n");
         rc = FALSE;
      }
   } else {
//      logprint ("scout: init_inet_daemon() failed\n");
      rc = FALSE;
   }
   return (rc);
}

int isCommand( STRPTR buffer, CONST_STRPTR command )
{
   int len = strlen (command);
   int rc = FALSE;

   if ((strnicmp (buffer, command, len) == 0) \
     && ((isspace (buffer[len])) || (buffer[len] == '\0'))) {
      rc = len + 1;
   }
   return (rc);
}

int VARARGS68K STDARGS SendClient( CONST_STRPTR fmt, ... )
{
    int   result = FALSE;
    VA_LIST args;
    STRPTR buf;

    VA_START(args,fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, VA_ARG(args, APTR))) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        if (send(server_socket, buf, strlen(buf) + 1, 0) == (LONG)strlen(buf) + 1) {
            result = TRUE;
        }

        FreeVec(buf);
    }

    VA_END(args);

    return result;
}

BOOL SendEncodedEntry( void *structure,
                       ULONG length )
{
    ULONG i;
    STRPTR s = (STRPTR)structure;

    for (i = 0; i < length; i++) {
        if (s[i] == '\0') s[i] = '\1';
    }

    return (BOOL)((send(server_socket, structure, length, 0) == (LONG)length) ? TRUE : FALSE);
}

long VARARGS68K STDARGS SendResultString( CONST_STRPTR fmt, ... )
{
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, VA_ARG(args, APTR))) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        if (serverstate) {
            strcat(buf, "\n");
            SendClient(buf);
        } else if (shellstate) {
            strcat(buf, "\n");
            PutStr(buf);
        } else {
            set(AP_Scout, MUIA_Application_RexxString, buf);
        }

        FreeVec(buf);
    }

    VA_END(args);

    return TRUE;
}

void VARARGS68K STDARGS PrintFOneLine( BPTR hd,
                                       CONST_STRPTR fmt, ... )
{
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, VA_ARG(args, APTR))) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        if (serverstate) {
            SendClient(buf);
        } else {
            Write(hd, buf, strlen(buf));
        }

        FreeVec(buf);
    }

    VA_END(args);
}

ULONG ExecuteCommand( STRPTR text )
{
   ULONG    rc = RETURN_OK;
   TEXT     buffer[256];
   int      i, len;

   #define  CMDOPT_TEMPLATE "CMD1/A"
   #define  CMDOPT_COUNT    4
   LONG     cmdopts[CMDOPT_COUNT];

   _snprintf(buffer, sizeof(buffer), "%s ", text);

   i = 0;
   while (arexx_list[i].mc_Name) {
      if ((len = isCommand (buffer, arexx_list[i].mc_Name)) > 0) {
         if ((myrdargs = AllocDosObject (DOS_RDARGS,NULL)) != NULL) {
            myrdargs->RDA_Source.CS_Buffer = buffer + len;
            myrdargs->RDA_Source.CS_Length = strlen (buffer) - len;

            if ((arexx_list[i].mc_Parameters && (rdargs = ReadArgs (arexx_list[i].mc_Template, (IPTR *) &cmdopts, myrdargs))) \
              || (!arexx_list[i].mc_Parameters)) {

               rc = CallHookPkt(arexx_list[i].mc_Hook, NULL, &cmdopts[0]);
               FreeArgs (rdargs);
            } else {

               Fault (IoErr(), NULL, (char *) buffer, LINELEN);
               SendResultString (buffer);
               rc = RETURN_FAIL;
            }
            FreeDosObject (DOS_RDARGS,myrdargs);
         }
         break;
      }
      i++;
   }

   if (! (arexx_list[i].mc_Name)) {
      i = 0;
      while (net_list[i].nc_Name) {
         if ((len = isCommand (buffer, net_list[i].nc_Name)) > 0) {
            if ((myrdargs = AllocDosObject (DOS_RDARGS,NULL)) != NULL) {
               myrdargs->RDA_Source.CS_Buffer = buffer;
               myrdargs->RDA_Source.CS_Length = strlen (buffer);

               if ((rdargs = ReadArgs (CMDOPT_TEMPLATE, (IPTR *) &cmdopts, myrdargs)) != NULL) {

                  (*net_list[i].nc_func) (NULL);
                  FreeArgs (rdargs);
               } else {

                  Fault (IoErr(), NULL, (char *) buffer, LINELEN);
                  SendResultString (buffer);
                  rc = RETURN_FAIL;
               }
               FreeDosObject (DOS_RDARGS,myrdargs);
            }
            break;
         }
         i++;
      }
      if (! (net_list[i].nc_Name)) {
         SendResultString(msgUnknownOption);
         rc = RETURN_FAIL;
      }
   }
   return (rc);
}

ULONG netdaemon (VOID) {
   ULONG    rc = TRUE;
   TEXT     buffer[LINELEN+2];
   int      len, done = FALSE;

   struct   passwd   *pw, *rootpw;
   long     rootid = 0;

/*
** inittcp() wurde bereits durch isNetCall() aufgerufen!
*/

   serverstate = TRUE;

   if ((rootpw = getpwnam ((char *)"root")) != NULL)
      rootid = rootpw->pw_gid;

   if ((sgets (server_socket, buffer, LINELEN)) && (strcmp (buffer, CMD_BEGIN) == 0)) {
//logprint ("BEGIN\n");
      if ((sgets (server_socket, buffer, LINELEN)) && (len = isCommand (buffer, CMD_USER)) \
        && (pw = getpwnam (buffer + len))) {
//logprint ("USER %s\n", buffer + len);

         if (((rootpw) && (rootid == pw->pw_gid)) || (! rootpw)) {

            if ((sgets (server_socket, buffer, LINELEN)) && (len = isCommand (buffer, CMD_PASSWORD))
              && (strcmp ((char *)crypt ((UBYTE *)buffer + len, (UBYTE *)pw->pw_passwd), pw->pw_passwd) == 0)) {
//logprint ("PASSWORD %s\n", buffer + len);

               if (SendClient (CMD_CONNECTED)) {
//logprint ("'CONNECTED' send!\n");
                  while (sgets (server_socket, buffer, LINELEN)) {
//logprint ("COMMAND '%s'\n", buffer);
                     if (strcmp (buffer, CMD_END) == 0) {
                        break;
                     }

                     if (ExecuteCommand (buffer) != RETURN_OK) {
                        SendClient (CMD_ERROR);
                     } else {
                        SendClient (CMD_DONE);
                     }
                     done = TRUE;
                  }
               }
            } else {
               SendClient(msgWrongPassword);
            }
         } else {
            SendClient(msgNoPrivilege);
         }
      } else {
         SendClient(msgUnknownUser);
      }
   } else {
      SendClient(msgNoBEGINReceived);
   }
   if (! done)
      SendClient (CMD_ERROR);

   serverstate = FALSE;
   return (rc);
}

ULONG VARARGS68K STDARGS MyDoCommand( CONST_STRPTR fmt, ... )
{
    ULONG result;
    VA_LIST args;
    STRPTR buf;

    VA_START(args, fmt);

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, VA_ARG(args, APTR))) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        if (clientstate) {
            result = NetCommand(buf);
        } else {
            result = ExecuteCommand(buf);
        }

        if (result != RETURN_OK) {
            MyRequest(msgErrorContinue, msgErrorOccured);
        }

        FreeVec(buf);
    } else {
        result = RETURN_FAIL;
    }

    VA_END(args);

    return result;
}

